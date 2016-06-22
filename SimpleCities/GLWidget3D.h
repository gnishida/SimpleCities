/*********************************************************************
This file is part of QtUrban.

    QtUrban is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    QtUrban is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtUrban.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#pragma once

#include "glew.h"
#include "VBORenderManager.h"
#include "GLWidget3D_Shadows.h"

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include "Camera.h"
#include "RoadGraph.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
public:
	MainWindow* mainWin;
	Camera camera;

	bool shiftPressed;
	bool ctrlPressed;
	bool altPressed;
	QPoint lastPos;

	VBORenderManager vboRenderManager;
	GLWidgetSimpleShadow shadow;

public:
	GLWidget3D(MainWindow* parent);

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void drawScene(int drawMode);
	void updateCamera();

protected:
	void initializeGL();

	void resizeGL(int width, int height);
	void paintGL();    

	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
};

