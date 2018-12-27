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

class FloorUserManager
{
public:
    vector<FloorUser> floorUsers;
    ofImage ringSprite;
    
    float canvasWidth;
    float canvasHeight;
    
    FloorUserManager()
    {
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
        glPushMatrix();
        glTranslatef(canvasWidth/2, canvasHeight/2,0);
        for(int i=0;i<floorUsers.size();i++)
        {
            floorUsers[i].drawRing(ringSprite);
        }
        glPopMatrix();
    }
};


#endif /* FloorUser_h */
