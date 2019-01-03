//
//  FloorUser.h
//  floor
//
//  Created by mb9 on 17/9/2018.
//

#ifndef FloorUser_h
#define FloorUser_h

extern float RING_PARTICLE_RADIUS_MIN;
extern float RING_PARTICLE_RADIUS_MAX;
extern float RING_PARTICLE_RADIUS_GROW_SPEED;
extern float RING_PARTICLE_RADIUS_DECAY_SPEED;
extern float RING_ORBIT_SPEED_MIN;
extern float RING_ORBIT_SPEED_MAX;
extern float RING_ORBIT_RADIUS;
extern float ORBIT_RADIUS_SCATTER;
extern float ORBIT_RADIUS_SCATTER_SPEED_MIN;
extern float ORBIT_RADIUS_SCATTER_SPEED_MAX;
extern int RING_PARTICLE_LIFE_MIN;
extern int RING_PARTICLE_LIFE_MAX;
extern float RING_PARTICLE_HUE_MIN;
extern float RING_PARTICLE_HUE_MAX;
extern float RING_PARTICLE_SATUATION;
extern float RING_PARTICLE_BRIGHTNESS;
extern float RING_PARTICLE_VIRTUAL_ALPHA;
extern int MAX_PARTICLE_PER_RING;


class FloorRingParticle{
public:
    float radius;
    float radiusTarget;
    float orbitAngle;
    float orbitSpeed;
    float orbitRadius;
    float orbitRadiusScatter;
    float orbitRadiusScatterSpeed;
    float chaseSpeed;
    int life;
    
    ofVec2f pos;
    ofVec2f posTarget;
    ofColor color;
    
    FloorRingParticle()
    {
        radius = 0;
        radiusTarget = pow(10,ofRandom(RING_PARTICLE_RADIUS_MIN,RING_PARTICLE_RADIUS_MAX));
        orbitAngle = ofRandom(TWO_PI);
        orbitSpeed = ofRandom(RING_ORBIT_SPEED_MIN,RING_ORBIT_SPEED_MAX);
        orbitRadius = RING_ORBIT_RADIUS;
        orbitRadiusScatter = pow(10,ofRandom(0,ORBIT_RADIUS_SCATTER));
        orbitRadiusScatterSpeed = pow(0.1,ofRandom(1,2));
        chaseSpeed = pow(0.1,ofRandom(ORBIT_RADIUS_SCATTER_SPEED_MIN,ORBIT_RADIUS_SCATTER_SPEED_MAX));
        life = ofRandom(RING_PARTICLE_LIFE_MIN,RING_PARTICLE_LIFE_MAX);
        color.setHsb(ofRandom(RING_PARTICLE_HUE_MIN,RING_PARTICLE_HUE_MAX), RING_PARTICLE_SATUATION, RING_PARTICLE_BRIGHTNESS);
    }
    
    void initPos(float targetX, float targetY)
    {
        posTarget.x = targetX + cos(orbitAngle) * (orbitRadius + orbitRadiusScatter * cos(orbitRadiusScatterSpeed * ofGetFrameNum()));
        posTarget.y = targetY + sin(orbitAngle) * (orbitRadius + orbitRadiusScatter * sin(orbitRadiusScatterSpeed * ofGetFrameNum()));
        pos = (posTarget);
    }
    
    void update(float targetX, float targetY)
    {
        radiusTarget *= RING_PARTICLE_RADIUS_DECAY_SPEED;
        radius += (radiusTarget - radius)*RING_PARTICLE_RADIUS_GROW_SPEED;
        orbitAngle += orbitSpeed;
        posTarget.x = targetX + cos(orbitAngle) * (orbitRadius + orbitRadiusScatter * cos(orbitRadiusScatterSpeed * ofGetFrameNum()));
        posTarget.y = targetY + sin(orbitAngle) * (orbitRadius + orbitRadiusScatter * sin(orbitRadiusScatterSpeed * ofGetFrameNum()));
        pos += (posTarget - pos)*chaseSpeed;
        life --;
    }
    
    void draw(const ofImage& sprite)
    {
        ofSetColor(color, RING_PARTICLE_VIRTUAL_ALPHA / radius * 100);
        sprite.draw(pos.x - radius * 0.5, pos.y - radius * 0.5, radius, radius);
    }
};

class FloorRing{
public:
    vector<FloorRingParticle> particles;
    
    FloorRing()
    {
    }
    
    void update(float targetX, float targetY, bool isEmitting = true)
    {
        if(particles.size()>MAX_PARTICLE_PER_RING)
        {
            particles.erase(particles.begin(),particles.begin()+particles.size()-MAX_PARTICLE_PER_RING);
        }
        
        if(isEmitting)
        {
            FloorRingParticle p;
            p.initPos(targetX,targetY);
            particles.push_back(p);
        }
        
        for(int i=0;i<particles.size();i++)
        {
            if(particles[i].life>0)
            {
                particles[i].update(targetX, targetY);
            }
            else
            {
                particles.erase(particles.begin()+i);
                i--;
            }
        }
    }
    
    void draw(const ofImage& sprite)
    {
        for(int i=0;i<particles.size();i++)
        {
            particles[i].draw(sprite);
        }
    }
};



class FloorUser
{
public:
    
    float dist1;
    float dist2;
    int numPts;
    float swingWidth;
    float swingHeight;
    float lightScale1;
    float lightScale2;
    ofVec2f pos;
    
    FloorRing ring;
    
    FloorUser()
    {
    }
    
    void update()
    {
        ring.update(pos.x,pos.y);
    }
    
    void drawRing(const ofImage& sprite){
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glPushMatrix();
            ring.draw(sprite);
        glPopMatrix();
    }
    
    void drawDebug()
    {
        ofSetColor(255,50);
        ofCircle(pos.x,pos.y,30);
    }
};

extern string RING_PARTICLE_SPRITE_PATH;
extern int MAX_USERS;
class FloorUserManager
{
public:
    vector<FloorUser> floorUsers;
    ofImage ringSprite;
    
    float canvasWidth;
    float canvasHeight;


	GLfloat *vertices;
	GLfloat *texs;
	GLfloat *colors;
    
    FloorUserManager()
    {
		vertices = new GLfloat[MAX_USERS * MAX_PARTICLE_PER_RING * 12];
		texs = new GLfloat[MAX_USERS * MAX_PARTICLE_PER_RING * 8];
		colors = new GLfloat[MAX_USERS * MAX_PARTICLE_PER_RING * 16];
		
    }
    
    void loadTextures()
    {
        ringSprite.load(RING_PARTICLE_SPRITE_PATH);
    }
    
    void addFloorUser(float x, float y)
    {
        FloorUser s;
        s.pos = ofVec2f(x,y);
        floorUsers.push_back(s);
    }
    
    void clearFloorUser()
    {
        floorUsers.clear();
    }
    
    void update()
    {
        for(int i=0;i<floorUsers.size();i++)
        {
            floorUsers[i].update();
        }
    }
    
    void draw(int mode)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glPushMatrix();
        glTranslatef(canvasWidth/2, canvasHeight/2,0);
        
        //old method
//        for(int i=0;i<floorUsers.size();i++)
//        {
//            floorUsers[i].drawRing(ringSprite);
//        }
        
        drawRingFast();
        glPopMatrix();
    }
    
    
    void drawRingFast()
    {
        int sumParticles = 0;
        
        for(int i=0;i<floorUsers.size();i++)
        {
            sumParticles += floorUsers[i].ring.particles.size();
        }
        
        
        int _particle = 0;
        
        for(int j=0;j<floorUsers.size();j++)
        {
            
            //prep arrays
            for(int i=0;i<floorUsers[j].ring.particles.size();i++)
            {
                float rr = floorUsers[j].ring.particles[i].radius;    //removed noise radius function
                ofVec3f p1 = ofVec3f(-rr*0.5, -rr*0.5);
                ofVec3f p2 = ofVec3f(-rr*0.5,  rr*0.5);
                ofVec3f p3 = ofVec3f( rr*0.5,  rr*0.5);
                ofVec3f p4 = ofVec3f( rr*0.5, -rr*0.5);
                
                //no rotation for ring particles
                
                p1 += floorUsers[j].ring.particles[i].pos;
                p2 += floorUsers[j].ring.particles[i].pos;
                p3 += floorUsers[j].ring.particles[i].pos;
                p4 += floorUsers[j].ring.particles[i].pos;
                
                vertices[(_particle+i)*12+0] = p1.x;
                vertices[(_particle+i)*12+1] = p1.y;
                vertices[(_particle+i)*12+2] = p1.z;
                vertices[(_particle+i)*12+3] = p2.x;
                vertices[(_particle+i)*12+4] = p2.y;
                vertices[(_particle+i)*12+5] = p2.z;
                vertices[(_particle+i)*12+6] = p3.x;
                vertices[(_particle+i)*12+7] = p3.y;
                vertices[(_particle+i)*12+8] = p3.z;
                vertices[(_particle+i)*12+9] = p4.x;
                vertices[(_particle+i)*12+10] = p4.y;
                vertices[(_particle+i)*12+11] = p4.z;
                
                texs[(_particle+i)*8+0] = 0;
                texs[(_particle+i)*8+1] = 0;
                texs[(_particle+i)*8+2] = 0;
                texs[(_particle+i)*8+3] = ringSprite.getHeight();
                texs[(_particle+i)*8+4] = ringSprite.getWidth();
                texs[(_particle+i)*8+5] = ringSprite.getHeight();
                texs[(_particle+i)*8+6] = ringSprite.getWidth();
                texs[(_particle+i)*8+7] = 0;
                
                colors[(_particle+i)*16+0] = floorUsers[j].ring.particles[i].color.r / 255.0;
                colors[(_particle+i)*16+1] = floorUsers[j].ring.particles[i].color.g / 255.0;
                colors[(_particle+i)*16+2] = floorUsers[j].ring.particles[i].color.b / 255.0;
                colors[(_particle+i)*16+3] = floorUsers[j].ring.particles[i].color.a / 255.0;
                colors[(_particle+i)*16+4] = floorUsers[j].ring.particles[i].color.r / 255.0;
                colors[(_particle+i)*16+5] = floorUsers[j].ring.particles[i].color.g / 255.0;
                colors[(_particle+i)*16+6] = floorUsers[j].ring.particles[i].color.b / 255.0;
                colors[(_particle+i)*16+7] = floorUsers[j].ring.particles[i].color.a / 255.0;
                colors[(_particle+i)*16+8] = floorUsers[j].ring.particles[i].color.r / 255.0;
                colors[(_particle+i)*16+9] = floorUsers[j].ring.particles[i].color.g / 255.0;
                colors[(_particle+i)*16+10] = floorUsers[j].ring.particles[i].color.b / 255.0;
                colors[(_particle+i)*16+11] = floorUsers[j].ring.particles[i].color.a / 255.0;
                colors[(_particle+i)*16+12] = floorUsers[j].ring.particles[i].color.r / 255.0;
                colors[(_particle+i)*16+13] = floorUsers[j].ring.particles[i].color.g / 255.0;
                colors[(_particle+i)*16+14] = floorUsers[j].ring.particles[i].color.b / 255.0;
                colors[(_particle+i)*16+15] = floorUsers[j].ring.particles[i].color.a / 255.0;
            }
            _particle += floorUsers[j].ring.particles.size();
        }
        ringSprite.getTexture().bind();
        
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, colors);
        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, texs);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        
        glDrawArrays(GL_QUADS, 0, sumParticles*4);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        ringSprite.getTexture().unbind();
    }
};


#endif /* FloorUser_h */
