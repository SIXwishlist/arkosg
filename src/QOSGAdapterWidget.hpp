/* OpenSceneGraph example, osganimate.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#ifndef QOSGAdapterWidget_H
#define QOSGAdapterWidget_H

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <QMutex>

#if USE_QT4

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QMdiArea>

using Qt::WindowFlags;

#else

class QWidget;
#include <qtimer.h>
#include <qgl.h>
#include <qapplication.h>

#define WindowFlags WFlags

#endif

#include <iostream>

class QOSGAdapterWidget : public QGLWidget
{
public:

    QOSGAdapterWidget( QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0 );

    virtual ~QOSGAdapterWidget() {}

    osgViewer::GraphicsWindow* getGraphicsWindow()
    {
        return _gw.get();
    }
    const osgViewer::GraphicsWindow* getGraphicsWindow() const
    {
        return _gw.get();
    }

protected:

    void init();

    virtual void resizeGL( int width, int height );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void keyReleaseEvent( QKeyEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );

    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
};

class ViewerQT : public osgViewer::Viewer, public QOSGAdapterWidget
{
public:

	QMutex mutex;

    ViewerQT(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0):
            QOSGAdapterWidget( parent, name, shareWidget, f )
    {
        getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
        getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width())/static_cast<double>(height()), 1.0f, 10000.0f);
        getCamera()->setGraphicsContext(getGraphicsWindow());

        setThreadingModel(osgViewer::Viewer::SingleThreaded);

        connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        _timer.start(100);
    }

    virtual void paintGL()
    {
		mutex.lock();
        frame();
		mutex.unlock();
    }

protected:

    QTimer _timer;
};

class CompositeViewerQT : public osgViewer::CompositeViewer, public QOSGAdapterWidget
{
public:

	QMutex mutex;

    CompositeViewerQT(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0):
            QOSGAdapterWidget( parent, name, shareWidget, f )
    {
        setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

        connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        _timer.start(100);
    }

    virtual void paintGL()
    {
		mutex.lock();
        frame();
		mutex.unlock();
    }

protected:

    QTimer _timer;
};

#endif

/*EOF*/
