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

#include "GLWidget3D.h"
#include "Util.h"
#include "GraphUtil.h"
#include "MainWindow.h"
#include <gl/GLU.h>
#include "VBOPm.h"
#include "BlockMeshGenerator.h"

GLWidget3D::GLWidget3D(MainWindow* mainWin) : QGLWidget(QGLFormat(QGL::SampleBuffers), (QWidget*)mainWin) {
	this->mainWin = mainWin;

	camera.resetCamera();

	ctrlPressed = false;
	shiftPressed = false;
	altPressed = false;

	camera.setRotation(0, 0, 0);
	camera.setTranslation(0, 0, G::getFloat("MAX_Z"));//6000);
}

void GLWidget3D::mousePressEvent(QMouseEvent *event) {
	QVector2D pos;

	if (Qt::ControlModifier == event->modifiers()) {
		ctrlPressed = true;
	} else {
		ctrlPressed = false;
	}

	this->setFocus();

	lastPos = event->pos();
}

void GLWidget3D::mouseReleaseEvent(QMouseEvent *event) {
}

void GLWidget3D::mouseMoveEvent(QMouseEvent *event) {
	/*QVector2D pos;
	mouseTo2D(event->x(), event->y(), pos);*/

	float dx = (float)(event->x() - lastPos.x());
	float dy = (float)(event->y() - lastPos.y());
	float camElevation = camera.getCamElevation();

	//vboRenderManager.mousePos3D=pos.toVector3D();
	
	if (ctrlPressed) {	// Rotate
		camera.changeXRotation(dy * 0.4);
		camera.changeZRotation(dx * 0.4);
		lastPos = event->pos();
	} else if (event->buttons() & Qt::LeftButton) {	// Translate
		camera.changeLookAt(-dx, dy, 0);
		lastPos = event->pos();
	}

	updateGL();
}

void GLWidget3D::wheelEvent(QWheelEvent* e) {
	camera.changeXYZTranslation(0, 0, -e->delta() * 0.2);

	updateGL();
}

void GLWidget3D::initializeGL() {
	//qglClearColor(QColor(113, 112, 117));
	qglClearColor(QColor(0, 0, 0));

	//---- GLEW extensions ----
	GLenum err = glewInit();
	if (GLEW_OK != err){// Problem: glewInit failed, something is seriously wrong.
		qDebug() << "Error: " << glewGetErrorString(err);
	}
	qDebug() << "Status: Using GLEW " << glewGetString(GLEW_VERSION);
	if (glewIsSupported("GL_VERSION_4_2")) {
		printf("Ready for OpenGL 4.2\n");
	} else {
		printf("OpenGL 4.2 not supported\n");
		exit(1);
	}
	const GLubyte* text= glGetString(GL_VERSION);
	printf("VERSION: %s\n",text);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glLineWidth(5.0f);
	glPointSize(10.0f);

	///
	vboRenderManager.init();
	shadow.initShadow(vboRenderManager.program,this);
	glUniform1i(glGetUniformLocation(vboRenderManager.program,"shadowState"), 0);//SHADOW: Disable
	glUniform1i(glGetUniformLocation(vboRenderManager.program, "terrainMode"), 0);//FLAT
}

void GLWidget3D::resizeGL(int width, int height) {
	camera.updatePerspective(width, height);
}

void GLWidget3D::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_TEXTURE_2D);
	
	updateCamera();

	drawScene(0);		
}

void GLWidget3D::drawScene(int drawMode) {
	if (G::getBool("shader2D")) {
		mainWin->urbanGeometry->render(vboRenderManager);
		
		vboRenderManager.vboTerrain.render();
		glLineWidth(1);
		vboRenderManager.renderStaticGeometry("2d_blocks_contour");
		vboRenderManager.renderStaticGeometry("2d_blocks");
		vboRenderManager.renderStaticGeometry("2d_parks");
		vboRenderManager.renderStaticGeometry("3d_roads");
	} else {
		if (drawMode == 0) {	// Regular rendering
			glUniform1i(glGetUniformLocation(vboRenderManager.program,"shadowState"), 0);
			vboRenderManager.renderStaticGeometry("sky");
			vboRenderManager.vboWater.render(vboRenderManager);
			mainWin->urbanGeometry->render(vboRenderManager);
			
			glUniform1i(glGetUniformLocation(vboRenderManager.program,"shadowState"), 1);
			vboRenderManager.vboTerrain.render();
			vboRenderManager.renderStaticGeometry("3d_blocks");
			vboRenderManager.renderStaticGeometry("3d_parks");
			vboRenderManager.renderStaticGeometry("3d_parcels");
			vboRenderManager.renderStaticGeometry("3d_roads");
			vboRenderManager.renderStaticGeometry("3d_building");

			vboRenderManager.renderAllStreetElementName("tree");
			vboRenderManager.renderAllStreetElementName("streetLamp");
		}
		else if (drawMode == 1) {	// Compute shadow mapping
			glUniform1i(glGetUniformLocation(vboRenderManager.program,"shadowState"), 2);// SHADOW: From light

			vboRenderManager.vboTerrain.render();

			vboRenderManager.renderStaticGeometry("3d_blocks");
			vboRenderManager.renderStaticGeometry("3d_parks");
			vboRenderManager.renderStaticGeometry("3d_parcels");
			vboRenderManager.renderStaticGeometry("3d_roads");
			vboRenderManager.renderStaticGeometry("3d_building");

			vboRenderManager.renderAllStreetElementName("tree");
			vboRenderManager.renderAllStreetElementName("streetLamp");
		}
	}
}

void GLWidget3D::keyPressEvent(QKeyEvent*e) {
	shiftPressed=false;
	ctrlPressed = false;
	altPressed=false;

	switch( e->key() ){
	case Qt::Key_Shift:
		shiftPressed = true;
		//clientMain->statusBar()->showMessage("Shift pressed");
		break;
	case Qt::Key_Control:
		ctrlPressed = true;
		break;
	case Qt::Key_Alt:
		altPressed=true;
		break;
	case Qt::Key_R:
		printf("Reseting camera pose\n");
		camera.resetCamera();
		break;
	default:
		;
	}
}

void GLWidget3D::keyReleaseEvent(QKeyEvent* e) {
	if (e->isAutoRepeat()) {
		e->ignore();
		return;
	}
	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Alt:
		altPressed = false;
		break;
	default:
		;
	}
}

// this method should be called after any camera transformation (perspective or modelview)
// it will update viewport, perspective, view matrix, and update the uniforms
void GLWidget3D::updateCamera() {
	// update matrices
	int height = this->height() ? this->height() : 1;
	glViewport(0, 0, (GLint)this->width(), (GLint)this->height());
	camera.updatePerspective(this->width(),height);

	// update uniforms
	float mvpMatrixArray[16];
	float mvMatrixArray[16];

	for(int i=0;i<16;i++){
		mvpMatrixArray[i]=camera.mvpMatrix.data()[i];
		mvMatrixArray[i]=camera.mvMatrix.data()[i];	
	}
	float normMatrixArray[9];
	for(int i=0;i<9;i++){
		normMatrixArray[i]=camera.normalMatrix.data()[i];
	}

	//glUniformMatrix4fv(mvpMatrixLoc,  1, false, mvpMatrixArray);
	glUniformMatrix4fv(glGetUniformLocation(vboRenderManager.program, "mvpMatrix"),  1, false, mvpMatrixArray);
	glUniformMatrix4fv(glGetUniformLocation(vboRenderManager.program, "mvMatrix"),  1, false, mvMatrixArray);
	glUniformMatrix3fv(glGetUniformLocation(vboRenderManager.program, "normalMatrix"),  1, false, normMatrixArray);

	// light direction
	QVector3D light_dir(-0.40f, 0.81f, -0.51f);
	glUniform3f(glGetUniformLocation(vboRenderManager.program, "lightDir"), light_dir.x(), light_dir.y(), light_dir.z());
}
