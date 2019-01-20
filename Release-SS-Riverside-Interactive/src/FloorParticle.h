//
//  FloorParticle.h
//  floor
//
//  Created by mb9 on 7/9/2018.
//

#include "ofMain.h"

#ifndef FloorParticle_h
#define FloorParticle_h

extern float PARTICLE_PATH_SCATTER;

extern float PARTICLE_RAISE_Z_CLIPPING;

class FloorParticle
{
public:
	ofVec3f pos;
	ofVec3f posSpeed;
	ofVec3f posAcc;

	ofQuaternion rot;
	ofQuaternion rotSpeed;
	ofQuaternion rotAcc;

	int seed;

	ofColor color;

	int life;

	float radius;
	float blingSpeed;

	FloorParticle()
	{
		life = 500;
		seed = (int)ofRandom(1000);
	}

	void randomize(bool is2dRotation = true)
	{
		double rotRange = PI;
		double rotRange2 = 5;

		if (is2dRotation)
		{
			rotSpeed = ofQuaternion(ofRandom(-rotRange2, rotRange2), ofVec3f(0, 0, ofRandom(-rotRange, rotRange)));
		}
		else
		{
			rotSpeed = ofQuaternion(ofRandom(-rotRange2, rotRange2), ofVec3f(ofRandom(-rotRange, rotRange), ofRandom(-rotRange, rotRange), ofRandom(-rotRange, rotRange)));
		}
	}
	void update()
	{
		life--;
		posSpeed += ofVec3f(ofRandom(-PARTICLE_PATH_SCATTER, PARTICLE_PATH_SCATTER), ofRandom(-PARTICLE_PATH_SCATTER, PARTICLE_PATH_SCATTER), ofRandom(-PARTICLE_PATH_SCATTER, PARTICLE_PATH_SCATTER));
		pos += posSpeed;
		rot = rot * rotSpeed;
	}

	void draw(const ofImage& sprite)
	{
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);

		float w, x, y, z;
		rot.getRotate(w, x, y, z);
		glRotatef(w, x, y, z);

		ofSetColor(color);
		ofFill();
		float rr = ofNoise(life*blingSpeed, blingSpeed)*(radius);

		sprite.draw(rr*-0.5, rr*-0.5, rr, rr);
		glPopMatrix();
	}
};

extern float EMITTER_EMIT_SPEED;
extern float EMITTER_LIFE_MIN;
extern float EMITTER_LIFE_MAX;

extern float PARTICLE_HUE_MIN;
extern float PARTICLE_HUE_MAX;
extern float PARTICLE_SATUATION;
extern float PARTICLE_BRIGHTNESS;

extern float PARTICLE_RADIUS;
extern float PARTICLE_RANGE;
extern float PARTICLE_SPEED_RANGE;

class FloorParticleEmitter
{
public:
	ofVec3f pos;
	ofVec3f posSpeed;
	ofQuaternion rot;

	ofColor color;
	int life;
	int radius;

	int emitSpeed;

	ofVec3f screenPos;

	ofRectangle boundingBox;

	FloorParticleEmitter()
	{
	}

	void randomize(bool is2dRotation = true)
	{
		emitSpeed = (int)ofRandom(1, EMITTER_EMIT_SPEED);
		life = (int)ofRandom(EMITTER_LIFE_MIN, EMITTER_LIFE_MAX);
		color.setHsb(ofRandom(PARTICLE_HUE_MIN, PARTICLE_HUE_MAX), PARTICLE_SATUATION, PARTICLE_BRIGHTNESS);
		radius = PARTICLE_RADIUS;

		float range = PARTICLE_RANGE;
		float speedRange = PARTICLE_SPEED_RANGE;
		pos = ofVec3f(ofRandom(-range, range), ofRandom(-range, range), ofRandom(-range, range));
		posSpeed = ofVec3f(ofRandom(-speedRange, speedRange), ofRandom(-speedRange, speedRange), ofRandom(-speedRange, speedRange)) * 0.2;
	}

	vector<FloorParticle> update(float capacity = 0)
	{
		vector<FloorParticle> particles;
		for (int i = 0; i < emitSpeed; i++)
		{
			if (ofRandom(1) > capacity)
			{
				FloorParticle p;
				p.randomize(true);
				p.radius = radius;
				p.pos = pos;
				p.color = color;
				particles.push_back(p);
			}
		}
		return particles;
	}

	void drawDebug()
	{
		ofSetColor(255, 0, 0);
		ofNoFill();
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		float w, x, y, z;
		rot.getRotate(w, x, y, z);
		glRotatef(w, x, y, z);
		ofRect(radius*-5, radius*-5, radius * 10, radius * 10);
		ofDrawBitmapString("x: " + ofToString(pos.x) + "\ny: " + ofToString(pos.y) + "\nz: " + ofToString(pos.z) + "\nradius: " + ofToString(radius), -radius * 5, -radius * 5 + 10);
		glPopMatrix();
	}

	bool hitTest(float x, float y, ofEasyCam *cam)
	{
		ofVec3f offset(radius * 5, -radius * 5, 0);
		ofVec3f cur = cam->worldToScreen(pos - offset);
		ofVec3f cur2 = cam->worldToScreen(pos + offset);

		boundingBox = ofRectangle(cur, cur2);

		return (x >= cur.x && x <= cur2.x && y >= cur.y && y <= cur2.y);
	}
};


extern string PARTICLE_SPRITE_PATH;
extern int MAX_PARTICLES;

extern float SCENE_GRAVITY;

class FloorParticleManager
{
public:
	vector<FloorParticleEmitter> emitters;
	float lastX, lastY;
	int maxParticle;

	float canvasWidth;
	float canvasHeight;

	vector<FloorParticle> particles;

	ofImage flakeSprite;


	GLfloat *vertices;
	GLfloat *texs;
	GLfloat *colors;


	FloorParticleManager()
	{
		maxParticle = MAX_PARTICLES;
		flakeSprite.load(PARTICLE_SPRITE_PATH);
		vertices = new GLfloat[MAX_PARTICLES * 12];
		texs = new GLfloat[MAX_PARTICLES * 8];
		colors = new GLfloat[MAX_PARTICLES * 16];
	}

	void checkBoundary()
	{
		for (int i = 0; i < particles.size(); i++)
		{
			if (particles[i].pos.x < -canvasWidth * 0.5 || particles[i].pos.x > canvasWidth*0.5 || particles[i].pos.y > canvasHeight*0.6 || particles[i].pos.z > PARTICLE_RAISE_Z_CLIPPING)
			{
				//out of boundaries
				particles.erase(particles.begin() + i);
				i--;
			}
		}
	}

	void update()
	{
		checkBoundary();
		for (int i = 0; i < emitters.size(); i++)
		{
			if (emitters[i].life > 0)
			{
				float capacity = particles.size() / (float)maxParticle;
				vector<FloorParticle> newParticles = emitters[i].update(capacity);
				particles.insert(particles.end(), newParticles.begin(), newParticles.end());
			}
			else
			{
				emitters.erase(emitters.begin() + i);
				i--;
			}
		}
		gravity(ofVec3f(0, SCENE_GRAVITY, 0));

		for (int i = 0; i < particles.size(); i++)
		{
			if (particles[i].life > 0)
				particles[i].update();
			else
			{
				particles.erase(particles.begin() + i);
				i--;
			}
		}
		if (particles.size() > maxParticle)
		{
			particles.erase(particles.begin(), particles.begin() + particles.size() - maxParticle);
		}
	}

	void drawParticles()
	{
		for (int i = 0; i < particles.size(); i++)
		{
			particles[i].draw(flakeSprite);
		}
	}

	void drawParticlesFast()
	{

		//prep arrays
		for (int i = 0; i < particles.size(); i++)
		{
			float rr = particles[i].radius;    //removed noise radius function
			ofVec3f p1 = ofVec3f(-rr * 0.5, -rr * 0.5);
			ofVec3f p2 = ofVec3f(-rr * 0.5, rr*0.5);
			ofVec3f p3 = ofVec3f(rr*0.5, rr*0.5);
			ofVec3f p4 = ofVec3f(rr*0.5, -rr * 0.5);

			p1 += particles[i].pos;
			p2 += particles[i].pos;
			p3 += particles[i].pos;
			p4 += particles[i].pos;

			vertices[i * 12 + 0] = p1.x;
			vertices[i * 12 + 1] = p1.y;
			vertices[i * 12 + 2] = p1.z;
			vertices[i * 12 + 3] = p2.x;
			vertices[i * 12 + 4] = p2.y;
			vertices[i * 12 + 5] = p2.z;
			vertices[i * 12 + 6] = p3.x;
			vertices[i * 12 + 7] = p3.y;
			vertices[i * 12 + 8] = p3.z;
			vertices[i * 12 + 9] = p4.x;
			vertices[i * 12 + 10] = p4.y;
			vertices[i * 12 + 11] = p4.z;

			texs[i * 8 + 0] = 0;
			texs[i * 8 + 1] = 0;
			texs[i * 8 + 2] = 0;
			texs[i * 8 + 3] = flakeSprite.getHeight();
			texs[i * 8 + 4] = flakeSprite.getWidth();
			texs[i * 8 + 5] = flakeSprite.getHeight();
			texs[i * 8 + 6] = flakeSprite.getWidth();
			texs[i * 8 + 7] = 0;

			colors[i * 16 + 0] = particles[i].color.r / 255.0;
			colors[i * 16 + 1] = particles[i].color.g / 255.0;
			colors[i * 16 + 2] = particles[i].color.b / 255.0;
			colors[i * 16 + 3] = particles[i].color.a / 255.0;
			colors[i * 16 + 4] = particles[i].color.r / 255.0;
			colors[i * 16 + 5] = particles[i].color.g / 255.0;
			colors[i * 16 + 6] = particles[i].color.b / 255.0;
			colors[i * 16 + 7] = particles[i].color.a / 255.0;
			colors[i * 16 + 8] = particles[i].color.r / 255.0;
			colors[i * 16 + 9] = particles[i].color.g / 255.0;
			colors[i * 16 + 10] = particles[i].color.b / 255.0;
			colors[i * 16 + 11] = particles[i].color.a / 255.0;
			colors[i * 16 + 12] = particles[i].color.r / 255.0;
			colors[i * 16 + 13] = particles[i].color.g / 255.0;
			colors[i * 16 + 14] = particles[i].color.b / 255.0;
			colors[i * 16 + 15] = particles[i].color.a / 255.0;
		}

		flakeSprite.getTexture().bind();

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, colors);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texs);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);

		glDrawArrays(GL_QUADS, 0, particles.size() * 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		flakeSprite.getTexture().unbind();

	}

	void drawEmitters()
	{
		for (int i = 0; i < emitters.size(); i++)
		{
			emitters[i].drawDebug();
		}
	}


	void drawFloor()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPushMatrix();
		glTranslatef(canvasWidth / 2, canvasHeight / 2, 0);
		drawParticlesFast();
		glPopMatrix();

		glDisable(GL_BLEND);
		ofEnableAlphaBlending();
	}

	void addParticle(int num = 1)
	{
		for (int i = 0; i < num; i++)
		{
			FloorParticle p;
			p.randomize(true);
			particles.push_back(p);
		}
	}

	void addEmitter(int num = 1)
	{
		for (int i = 0; i < num; i++)
		{
			FloorParticleEmitter e;
			e.randomize(true);
			emitters.push_back(e);
		}
	}

	void addTopEmitter(int num = 5, int duplicate = 1)
	{
		for (int i = 0; i < num; i++)
		{
			float x = (canvasWidth / (float)num * (i + 0.5) - canvasWidth / 2);
			float y = -canvasHeight / 2 - 200;

			for (int k = 0; k < duplicate; k++)
			{
				FloorParticleEmitter e;
				e.randomize(true);
				e.pos = ofVec3f(x, y, 0);
				emitters.push_back(e);
			}
		}
	}

	void addBottomEmitter(int num = 5, int duplicate = 1)
	{
		for (int i = 0; i < num; i++)
		{
			float x = (canvasWidth / (float)num * (i + 0.5) - canvasWidth / 2);
			float y = canvasHeight * 0.55;

			for (int k = 0; k < duplicate; k++)
			{
				FloorParticleEmitter e;
				e.randomize(true);
				e.pos = ofVec3f(x, y, 0);
				emitters.push_back(e);
			}
		}
	}
	void clearEmitter()
	{
		emitters.erase(emitters.begin(), emitters.begin() + emitters.size());
	}

	void mousePressed(float x, float y)
	{

	}

	void mouseDragged(float x, float y)
	{

	}

	void mouseReleased(float x, float y)
	{
	}

	void gravity(ofVec3f dir)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			particles[i].posSpeed += dir;
		}
	}

	void repel(ofVec3f repeller, float force, float scatter)
	{
		float G = force + ofRandom(-scatter, scatter);
		for (int i = 0; i < particles.size(); i++)
		{
			ofVec3f dd = repeller - particles[i].pos;
			ofVec3f dir = ofVec3f(dd.x, dd.y);
			float mag = dir.length();
			dir.normalize();
			float f = -1 * G / (mag * mag);
			dir *= f;
			particles[i].posSpeed += dir;
		}
	}

	void massRepel(vector<ofVec3f> repellers, float force, float scatter)
	{
		for (int i = 0; i < repellers.size(); i++)
		{
			repel(repellers[i], force, scatter);
		}
	}

	void raise(float min, float max)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			ofVec3f dir = ofVec3f(0, 0, ofRandom(min, max));
			particles[i].posSpeed += dir;
		}
	}

	void attract(ofVec3f attractor, float force, float scatter)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			ofVec3f diff = attractor - particles[i].pos + ofVec3f(ofRandom(-scatter, scatter), ofRandom(-scatter, scatter), ofRandom(-scatter, scatter));
			float mag = diff.length();
			diff.normalize();
			diff = diff * (1 / mag) * 300;
			diff.limit(5);
			diff *= force;
			particles[i].posSpeed += diff;
		}
	}

	void massAttract(vector<ofVec3f> attractors, float force, float scatter)
	{
		for (int i = 0; i < attractors.size(); i++)
		{
			for (int j = 0; j < particles.size(); j++)
			{
				ofVec3f diff = attractors[i] - particles[j].pos + ofVec3f(ofRandom(-scatter, scatter), ofRandom(-scatter, scatter), ofRandom(-scatter, scatter));
				float mag = diff.length();
				diff.normalize();
				diff = diff * (1 / (mag - 100)) * 100;
				diff.limit(0.03);
				diff *= force;
				if (particles[j].seed%attractors.size() == i)
				{
					diff *= 55;
					particles[j].posAcc += diff;
					particles[j].posSpeed += particles[j].posAcc;
				}
			}
		}
	}

	void damp(float damping)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			particles[i].posAcc *= damping;
			particles[i].posSpeed *= damping;
		}
	}
};

#endif /* FloorParticle_h */
