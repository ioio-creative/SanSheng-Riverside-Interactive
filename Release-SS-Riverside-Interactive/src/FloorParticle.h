//
//  FloorParticle.h
//  floor
//
//  Created by mb9 on 7/9/2018.
//

#ifndef FloorParticle_h
#define FloorParticle_h
#include "ofMain.h"
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
        life = (int)ofRandom(100,500);
        blingSpeed = pow(0.1,ofRandom(1,2));
        color.setHsb(ofRandom(255),255,255);
        radius = 20;
        
        float range = 1000;
        pos = ofVec3f(ofRandom(-range,range),ofRandom(-range,range),ofRandom(-range,range));
        
        float speedRange = 25;
        posSpeed = ofVec3f(ofRandom(-speedRange,speedRange),ofRandom(-speedRange,speedRange),ofRandom(-speedRange,speedRange)) * 0.2;
        
        
        double rotRange = PI;
        double rotRange2 = 5;
        
        if(is2dRotation)
        {
            rotSpeed = ofQuaternion(ofRandom(-rotRange2,rotRange2),ofVec3f(0,0,ofRandom(-rotRange,rotRange)));
        }
        else
        {
            rotSpeed = ofQuaternion(ofRandom(-rotRange2,rotRange2),ofVec3f(ofRandom(-rotRange,rotRange),ofRandom(-rotRange,rotRange),ofRandom(-rotRange,rotRange)));
        }
    }
    
    void update()
    {
        life--;
        pos += posSpeed;
        rot = rot * rotSpeed;
    }
    
    void draw()
    {
        glPushMatrix();
            glTranslatef(pos.x,pos.y,pos.z);

            float w,x,y,z;
            rot.getRotate(w,x,y,z);
            glRotatef(w,x,y,z);
        
            ofSetColor(color);
            ofFill();
            float rr = ofNoise(life*blingSpeed,blingSpeed)*(radius);
            ofRectMode(OF_RECTMODE_CENTER);
            ofRect(0,0, rr, rr);
        
        glPopMatrix();
    }
};


class FloorParticleEmitterOrbit
{
public:
    ofVec3f pos;
    ofVec3f posSpeed;
    
    ofColor color;
    int radius;
    
    int emitSpeed;
    float orbitRadius;
    float orbitOffsetX;
    float orbitOffsetY;
    float orbitSpeedX;
    float orbitSpeedY;
    ofVec3f origin;
    FloorParticleEmitterOrbit()
    {
        orbitRadius = 100;
        emitSpeed = 5;
    }
    
    vector<FloorParticle> update(float capacity = 0)
    {
        orbitOffsetX += orbitSpeedX;
        orbitOffsetY += orbitSpeedY;
        pos = origin + ofVec3f(cos(orbitOffsetX)*orbitRadius, sin(orbitOffsetY)*orbitRadius, 0);

        posSpeed = ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)) * 0.2;
        vector<FloorParticle> particles;
        for(int i=0;i<emitSpeed;i++)
        {
            if(ofRandom(1)>capacity)
            {
                FloorParticle p;
                p.randomize(true);
                p.radius = radius;
                p.pos = pos;
                p.posSpeed = posSpeed;
                p.color = color;
                p.life = ofRandom(20,50);
                particles.push_back(p);
            }
        }
        return particles;
    }
    
};

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
    
    vector<FloorParticleEmitterOrbit> children;
    
    
    FloorParticleEmitter()
    {
    }
    
    void randomize(bool is2dRotation = true)
    {
        emitSpeed = (int)ofRandom(1,4);
        life = (int)ofRandom(300,1000);
        color.setHsb(ofRandom(0,10)+160,230,255);
        radius = 30;
        
        float range = 400;
        float speedRange = 5;
        pos = ofVec3f(ofRandom(-range,range),ofRandom(-range,range),ofRandom(-range,range));
        posSpeed = ofVec3f(ofRandom(-speedRange,speedRange),ofRandom(-speedRange,speedRange),ofRandom(-speedRange,speedRange)) * 0.2;
        
        int numOrbit = 0;//(int)ofRandom(2,6.99);
        float orbitSpeedX = pow(0.1,ofRandom(1,2));
        float orbitSpeedY = pow(0.1,ofRandom(1,2));
        float orbitRadius = ofRandom(30,50);
        for(int i=0;i<numOrbit;i++)
        {
            FloorParticleEmitterOrbit o;
            o.orbitOffsetX = TWO_PI/numOrbit*i;
            o.orbitOffsetY = TWO_PI/numOrbit*i;
            o.orbitSpeedX = orbitSpeedX;
            o.orbitSpeedY = orbitSpeedY;
            o.orbitRadius = orbitRadius;
            o.color = ofColor(color,50);
            o.radius = radius*0.5;
            o.posSpeed = posSpeed*0.3;
            
            children.push_back(o);
        }
    }
    
    vector<FloorParticle> update(float capacity = 0)
    {
        vector<FloorParticle> particles;
        for(int i=0;i<emitSpeed;i++)
        {
            if(ofRandom(1)>capacity)
            {
                FloorParticle p;
                p.randomize(true);
                p.radius = radius;
                p.pos = pos;
                p.color = color;
                particles.push_back(p);
            }
        }
        
        for(int i=0;i<children.size();i++)
        {
            children[i].origin = pos;
            vector<FloorParticle> newParticles = children[i].update(capacity);
            particles.insert(particles.end(),newParticles.begin(),newParticles.end());
        }
        return particles;
    }
    
    void draw()
    {
        ofSetColor(255,0,0);
        ofNoFill();
        glPushMatrix();
            glTranslatef(pos.x,pos.y,pos.z);
            float w,x,y,z;
            rot.getRotate(w,x,y,z);
            glRotatef(w,x,y,z);
            ofRect(radius*-5, radius*-5, radius*10, radius*10);
        ofDrawBitmapString("x: "+ofToString(pos.x)+"\ny: "+ofToString(pos.y)+"\nz: "+ofToString(pos.z)+"\nradius: "+ofToString(radius),-radius*5,-radius*5+10);
        glPopMatrix();
        
    }
    
    bool hitTest(float x, float y,ofEasyCam *cam)
    {
        ofVec3f offset(radius*5,-radius*5,0);
        ofVec3f cur = cam->worldToScreen(pos - offset);
        ofVec3f cur2 = cam->worldToScreen(pos + offset);
        
        boundingBox = ofRectangle(cur,cur2);
        
        return (x >= cur.x && x <= cur2.x && y >= cur.y && y <=cur2.y);
    }
    
    
};



class FloorParticleManager
{
public:
    vector<FloorParticleEmitter> emitters;
    float lastX, lastY;
    int maxParticle;
    
    float canvasWidth;
    float canvasHeight;
    
    vector<FloorParticle> particles;
    
    
    FloorParticleManager()
    {
        maxParticle = 8000;
    }
    
    void update()
    {
        
        for(int i=0;i<emitters.size();i++)
        {
            if(emitters[i].life>0)
            {
                float capacity = particles.size()/ (float)maxParticle;
                vector<FloorParticle> newParticles = emitters[i].update(capacity);
                particles.insert(particles.end(),newParticles.begin(),newParticles.end());
            }
            else
            {
                emitters.erase(emitters.begin()+i);
                i--;
            }
        }
        gravity(ofVec3f(0,0.2,0));
        
        for(int i=0;i<particles.size();i++)
        {
            if(particles[i].life>0)
                particles[i].update();
            else
            {
                particles.erase(particles.begin()+i);
                i--;
            }
        }
        if(particles.size()>maxParticle)
        {
            particles.erase(particles.begin(),particles.begin()+particles.size()-maxParticle);
        }
    }
    
    void drawParticles()
    {
        for(int i=0;i<particles.size();i++)
        {
            particles[i].draw();
        }
    }
    
    void drawEmitters()
    {
        for(int i=0;i<emitters.size();i++)
        {
            emitters[i].draw();
        }
    }
    
    
    void drawFloor()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glPushMatrix();
        glTranslatef(canvasWidth/2,canvasHeight/2,0);
        drawParticles();
        glPopMatrix();
        
        glDisable(GL_BLEND);
        ofEnableAlphaBlending();

    }
    
    void addParticle(int num=1)
    {
        for(int i=0;i<num;i++)
        {
            FloorParticle p;
            p.randomize(true);
            particles.push_back(p);
        }
    }
    
    void addEmitter(int num=1)
    {
        for(int i=0;i<num;i++)
        {
            FloorParticleEmitter e;
            e.randomize(true);
            emitters.push_back(e);
        }
    }
    
    void addTopEmitter(int num = 5)
    {
        for(int i=0;i<num;i++)
        {
            float x = (canvasWidth / (float)num * (i+0.5) - canvasWidth/2);
            float y = -canvasHeight/2-200;
            FloorParticleEmitter e;
            e.randomize(true);
            e.pos = ofVec3f(x,y,0);
            emitters.push_back(e);
        }
    }
    
    void clearEmitter()
    {
        emitters.erase(emitters.begin(),emitters.begin()+emitters.size());
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
        for(int i=0;i<particles.size();i++)
        {
            particles[i].posSpeed += dir;
        }
    }
    
    void repel(ofVec3f repeller, float force, float scatter)
    {
        float G = force + ofRandom(-scatter,scatter);
        for(int i=0;i<particles.size();i++)
        {
            ofVec3f dd = repeller - particles[i].pos;
            ofVec2f dir = ofVec2f(dd.x,dd.y);
            float mag = dir.length() + 30;
            dir.normalize();
            float f = -1 * G / (mag * mag);
            dir *= f;
            
            particles[i].posSpeed += dir;
        }
    }
    
    void massRepel(vector<ofVec3f> repellers, float force, float scatter)
    {
        for(int i=0;i<repellers.size();i++)
        {
            repel(repellers[i],force,scatter);
        }
    }
    
    void attract(ofVec3f attractor, float force,float scatter)
    {
        for(int i=0;i<particles.size();i++)
        {
            ofVec3f diff = attractor - particles[i].pos + ofVec3f(ofRandom(-scatter,scatter),ofRandom(-scatter,scatter),ofRandom(-scatter,scatter));
            float mag = diff.length();
            diff.normalize();
            diff = diff * (1/mag) * 100;
//            diff = diff * diff;
            diff.limit(5);
            diff *= force;
            particles[i].posSpeed += diff;
        }
    }
    
    void massAttract(vector<ofVec3f> attractors, float force, float scatter)
    {
        for(int i=0;i<attractors.size();i++)
        {
            for(int j=0;j<particles.size();j++)
            {
                ofVec3f diff = attractors[i] - particles[j].pos + ofVec3f(ofRandom(-scatter,scatter),ofRandom(-scatter,scatter),ofRandom(-scatter,scatter));
                float mag = diff.length();
                diff.normalize();
                diff = diff * (1/mag) * 100;
                diff.limit(5);
                diff *= force;
                if(particles[j].seed%attractors.size() == i)
                {
                    diff *= 55;
                }
                else diff /= 100;
                particles[j].posSpeed += diff;
            }
        }
    }
    
    
    
    void damp(float damping)
    {
        for(int i=0;i<particles.size();i++)
        {
            particles[i].posSpeed *= damping;
        }
    }
};



#endif /* FloorParticle_h */
