//
//  FloorUser.h
//  floor
//
//  Created by mb9 on 17/9/2018.
//

#ifndef FloorUser_h
#define FloorUser_h

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
    
    
    FloorUser()
    {
        dist1 = -20;
        dist2 = -550;
        numPts = 7;
        swingWidth = 25;
        swingHeight = 3;
        lightScale1 = 1;
        lightScale2 = 5;
    }
    
    
    void draw(const ofImage& texture, int mode)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        if(mode == 2)
        {
            for(int i=0;i<numPts;i++)
            {
                
                float angle1 = TWO_PI/(float)numPts * i + ofGetFrameNum()*0.01;
                float angle2 = TWO_PI/(float)numPts * (i+1) + ofGetFrameNum()*0.01;
                float offsetX1 = cos(angle1)*swingWidth;
                float offsetY1 = sin(angle1)*swingHeight;
                float offsetX2 = cos(angle2)*swingWidth;
                float offsetY2 = sin(angle2)*swingHeight;
                
                ofVec2f p1,p2,p3,p4;
                
                p1 = ofVec2f(offsetX1*lightScale1, offsetY1*lightScale1 + dist1) + pos;
                p2 = ofVec2f(offsetX1*lightScale2, offsetY1*lightScale2 + dist2) + pos;
                p3 = ofVec2f(offsetX2*lightScale2, offsetY2*lightScale2 + dist2) + pos;
                p4 = ofVec2f(offsetX2*lightScale1, offsetY2*lightScale1 + dist1) + pos;
                
                texture.getTexture().bind();
                float textureWidth = texture.getWidth();
                float textureHeight = texture.getHeight();
                ofSetColor(255);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex2f(p1.x, p1.y);
                glTexCoord2f(0, textureHeight);
                glVertex2f(p2.x, p2.y);
                glTexCoord2f(textureWidth, textureHeight);
                glVertex2f(p3.x, p3.y);
                glTexCoord2f(textureWidth, 0);
                glVertex2f(p4.x, p4.y);
                glEnd();
                
                texture.getTexture().unbind();
            }
        }
        ofSetColor(255,50);
        ofCircle(pos.x,pos.y,30);
    }
};

class FloorUserManager
{
public:
    ofImage lightTexture[2];
    vector<FloorUser> floorUsers;
    
    float canvasWidth;
    float canvasHeight;
    
    FloorUserManager()
    {
    }
    
    void loadTextures()
    {
        for(int i=0;i<2;i++)
        {
            lightTexture[i].load("lightTexture/light"+ofToString(i+1)+".png");
        }
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
        
    }
    
    void draw(int mode)
    {
        glPushMatrix();
        glTranslatef(canvasWidth/2, canvasHeight/2,0);
        for(int i=0;i<floorUsers.size();i++)
        {
            floorUsers[i].draw(lightTexture[0], mode);
        }
        glPopMatrix();
    }
    
};


#endif /* FloorUser_h */
