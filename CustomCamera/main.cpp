#include <iostream>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/CameraManipulator>
//���������
class ZoomManipulator : public osgGA::CameraManipulator
{
public:
	//���캯������ڵ�����Χ��
	ZoomManipulator(osg::Node *node);
	~ZoomManipulator();

	//����������������ʵ�ֵ�4�����麯��
	virtual void setByMatrix(const osg::Matrixd& matrix) {}
	virtual void setByInverseMatrix(const osg::Matrixd& matrix) {}
	virtual osg::Matrixd getMatrix() const { return osg::Matrix(); }
	virtual osg::Matrixd getInverseMatrix() const;

	//��ȡ����ڵ㣬����ʹ��CameraManipulator�е�computeHomePosition
	virtual const osg::Node* getNode() const { return _root; }
	virtual osg::Node* getNode() { return _root; }

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	osg::Vec3	_eye;				//�ӵ�λ��
	osg::Vec3   _direction;         //�ӵ㷽��
	osg::Vec3	_up;                //���Ϸ���
	osg::Node*	_root;
};

ZoomManipulator::ZoomManipulator(osg::Node *node)
{
	_root = node;
	computeHomePosition();

	_eye = _homeEye;
	_direction = _homeCenter - _homeEye;
	_up = _homeUp;
}

ZoomManipulator::~ZoomManipulator()
{

}

osg::Matrixd ZoomManipulator::getInverseMatrix() const
{
	osg::Matrix mat;
	mat.makeLookAt(_eye, _eye + _direction, _up);

	return mat;
}


bool ZoomManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	switch (ea.getEventType())
	{

	case (osgGA::GUIEventAdapter::SCROLL):
	{
		osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&us);
		osg::Camera *camera = viewer->getCamera();
		osg::Matrix MVPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();

		osg::Matrix inverseMVPW = osg::Matrix::inverse(MVPW);
		osg::Vec3 mouseWorld = osg::Vec3(ea.getX(), ea.getY(), 0) * inverseMVPW;

		osg::Vec3 direction = mouseWorld - _eye;
		direction.normalize();

		if (ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP)
		{
			_eye += direction * 20.0;
		}
		else if (ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN)
		{
			_eye -= direction * 20.0;
		}
	}

	default:
		return false;
	}
}



int main(int argc, char** argv)
{
	osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("cessna.osg");
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(50, 50, 800, 600);
	viewer.setCameraManipulator(new ZoomManipulator(root.get()));
	viewer.setSceneData(root.get());
	return viewer.run();
}
