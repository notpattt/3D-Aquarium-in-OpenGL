#include "Geometry.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <cmath>

std::vector<float> buildFishVertices() {
    std::vector<float> v;
    auto tri = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 ua, glm::vec2 ub, glm::vec2 uc) {
        glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));
        for (auto& pv : std::vector<std::pair<glm::vec3,glm::vec2>>{{a,ua},{b,ub},{c,uc}}) {
            v.push_back(pv.first.x); v.push_back(pv.first.y); v.push_back(pv.first.z);
            v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
            v.push_back(pv.second.x); v.push_back(pv.second.y);
        }
    };

    // body
    auto bodyPt = [](float t, float a) { float sx=sinf(glm::pi<float>()*t); return glm::vec3(t-.5f, .18f*sx*sinf(a), .09f*sx*cosf(a)); };
    for (int i=0;i<20;i++) for (int j=0;j<16;j++) {
        float t0=(float)i/20,t1=(float)(i+1)/20,a0=(float)j/16*glm::two_pi<float>(),a1=(float)(j+1)/16*glm::two_pi<float>();
        tri(bodyPt(t0,a0),bodyPt(t1,a0),bodyPt(t1,a1),{t0,(float)j/16},{t1,(float)j/16},{t1,(float)(j+1)/16});
        tri(bodyPt(t0,a0),bodyPt(t1,a1),bodyPt(t0,a1),{t0,(float)j/16},{t1,(float)j/16},{t0,(float)(j+1)/16});
    }

    // tail
    glm::vec3 tl{-.5f,0,0},t1{-.78f,.22f,0},t2{-.78f,-.22f,0},tm{-.62f,0,0};
    tri(tl,tm,t1,{0,0},{.5f,.5f},{0,1}); tri(tl,t2,tm,{0,0},{0,1},{.5f,.5f});
    tri(t1,tm,tl,{0,1},{.5f,.5f},{0,0}); tri(tm,t2,tl,{.5f,.5f},{0,1},{0,0});

    return v;
}


std::vector<float> buildSeaweedBlade(glm::vec3 root, float height, float width, int segs) {
    std::vector<float> v;

    float thick = width * 0.55f; // grubość liścia

    auto pushTri = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c,
                        glm::vec2 ua, glm::vec2 ub, glm::vec2 uc) {
        glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));
        v.push_back(a.x); v.push_back(a.y); v.push_back(a.z);
        v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
        v.push_back(ua.x); v.push_back(ua.y);
        v.push_back(b.x); v.push_back(b.y); v.push_back(b.z);
        v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
        v.push_back(ub.x); v.push_back(ub.y);
        v.push_back(c.x); v.push_back(c.y); v.push_back(c.z);
        v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
        v.push_back(uc.x); v.push_back(uc.y);
    };

    for (int i = 0; i < segs; i++) {
        float t0 = (float)i     / segs;
        float t1 = (float)(i+1) / segs;

        float y0 = root.y + t0 * height;
        float y1 = root.y + t1 * height;

        float hw0 = width * 0.5f * (1.f - t0 * 0.75f);
        float hw1 = width * 0.5f * (1.f - t1 * 0.75f);
        float ht0 = thick * 0.5f * (1.f - t0 * 0.8f);
        float ht1 = thick * 0.5f * (1.f - t1 * 0.8f);

        float bend0 = sinf(t0 * glm::pi<float>()) * width * 0.2f;
        float bend1 = sinf(t1 * glm::pi<float>()) * width * 0.2f;

        glm::vec3 FL0 = {root.x - hw0, y0, root.z - ht0 + bend0};
        glm::vec3 FR0 = {root.x + hw0, y0, root.z - ht0 + bend0};
        glm::vec3 BL0 = {root.x - hw0, y0, root.z + ht0 + bend0};
        glm::vec3 BR0 = {root.x + hw0, y0, root.z + ht0 + bend0};

        glm::vec3 FL1 = {root.x - hw1, y1, root.z - ht1 + bend1};
        glm::vec3 FR1 = {root.x + hw1, y1, root.z - ht1 + bend1};
        glm::vec3 BL1 = {root.x - hw1, y1, root.z + ht1 + bend1};
        glm::vec3 BR1 = {root.x + hw1, y1, root.z + ht1 + bend1};

        // front wall
        pushTri(FL0, FR0, FR1, {0,t0},{1,t0},{1,t1});
        pushTri(FL0, FR1, FL1, {0,t0},{1,t1},{0,t1});
        // back wall
        pushTri(BR0, BL0, BL1, {0,t0},{1,t0},{1,t1});
        pushTri(BR0, BL1, BR1, {0,t0},{1,t1},{0,t1});
        // left wall
        pushTri(BL0, FL0, FL1, {0,t0},{1,t0},{1,t1});
        pushTri(BL0, FL1, BL1, {0,t0},{1,t1},{0,t1});
        // right wall
        pushTri(FR0, BR0, BR1, {0,t0},{1,t0},{1,t1});
        pushTri(FR0, BR1, FR1, {0,t0},{1,t1},{0,t1});
    }

    return v;
}

std::vector<float> buildBranchCoral(float trunkH, float trunkR, int numBranches, float branchLen, float branchR) {
    std::vector<float> v;
    const int SEG=12;
    auto cylinder = [&](glm::vec3 base, glm::vec3 top, float r) {
        glm::vec3 axis = glm::normalize(top-base);
        glm::vec3 perp = glm::normalize(glm::cross(axis, fabsf(axis.x)<.9f?glm::vec3(1,0,0):glm::vec3(0,1,0)));
        glm::vec3 perp2= glm::cross(axis, perp);
        for (int i=0;i<SEG;i++) {
            float a0=(float)i/SEG*glm::two_pi<float>(), a1=(float)(i+1)/SEG*glm::two_pi<float>();
            glm::vec3 B0=base+(cosf(a0)*perp+sinf(a0)*perp2)*r;
            glm::vec3 B1=base+(cosf(a1)*perp+sinf(a1)*perp2)*r;
            glm::vec3 T0=top +(cosf(a0)*perp+sinf(a0)*perp2)*r;
            glm::vec3 T1=top +(cosf(a1)*perp+sinf(a1)*perp2)*r;
            glm::vec3 n0=glm::normalize(cosf(a0)*perp+sinf(a0)*perp2);
            glm::vec3 n1=glm::normalize(cosf(a1)*perp+sinf(a1)*perp2);
            for (auto& pv : std::vector<std::pair<glm::vec3,glm::vec3>>{{B0,n0},{B1,n1},{T1,n1}})
                { v.push_back(pv.first.x);v.push_back(pv.first.y);v.push_back(pv.first.z);v.push_back(pv.second.x);v.push_back(pv.second.y);v.push_back(pv.second.z);v.push_back(.5f);v.push_back(.5f); }
            for (auto& pv : std::vector<std::pair<glm::vec3,glm::vec3>>{{B0,n0},{T1,n1},{T0,n0}})
                { v.push_back(pv.first.x);v.push_back(pv.first.y);v.push_back(pv.first.z);v.push_back(pv.second.x);v.push_back(pv.second.y);v.push_back(pv.second.z);v.push_back(.5f);v.push_back(.5f); }
        }
    };
    cylinder({0,0,0},{0,trunkH,0}, trunkR);
    for (int i=0;i<numBranches;i++) {
        float angle = (float)i/numBranches * glm::two_pi<float>();
        float heightFrac = .4f + (float)i/numBranches * .5f;
        glm::vec3 brBase(0, trunkH*heightFrac, 0);
        glm::vec3 dir = glm::normalize(glm::vec3(cosf(angle), .6f, sinf(angle)));
        glm::vec3 brTop = brBase + dir * branchLen;
        cylinder(brBase, brTop, branchR);
        for (int k=0;k<2;k++) {
            float a2 = angle + (k==0?0.5f:-0.5f);
            glm::vec3 subDir = glm::normalize(glm::vec3(cosf(a2), .8f, sinf(a2)));
            cylinder(brTop, brTop+subDir*(branchLen*.5f), branchR*.6f);
        }
    }
    return v;
}

std::vector<float> buildMushroomCoral(float stalkH, float stalkR, float capR, float capH) {
    std::vector<float> v;
    const int SEG=24;
    auto push = [&](glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
        v.push_back(p.x);v.push_back(p.y);v.push_back(p.z);
        v.push_back(n.x);v.push_back(n.y);v.push_back(n.z);
        v.push_back(uv.x);v.push_back(uv.y);
    };
    for (int i=0;i<SEG;i++) {
        float a0=(float)i/SEG*glm::two_pi<float>(), a1=(float)(i+1)/SEG*glm::two_pi<float>();
        glm::vec3 B0(stalkR*cosf(a0),0,stalkR*sinf(a0)), B1(stalkR*cosf(a1),0,stalkR*sinf(a1));
        glm::vec3 T0(stalkR*cosf(a0),stalkH,stalkR*sinf(a0)), T1(stalkR*cosf(a1),stalkH,stalkR*sinf(a1));
        glm::vec3 n0(cosf(a0),0,sinf(a0)), n1(cosf(a1),0,sinf(a1));
        push(B0,n0,{(float)i/SEG,0}); push(B1,n1,{(float)(i+1)/SEG,0}); push(T1,n1,{(float)(i+1)/SEG,1});
        push(B0,n0,{(float)i/SEG,0}); push(T1,n1,{(float)(i+1)/SEG,1}); push(T0,n0,{(float)i/SEG,1});
    }
    for (int i=0;i<SEG;i++) {
        float a0=(float)i/SEG*glm::two_pi<float>(), a1=(float)(i+1)/SEG*glm::two_pi<float>();
        float r0=0, r1=capR*.45f, r2=capR*.8f, r3=capR;
        float h0=stalkH+capH, h1=stalkH+capH*.6f, h2=stalkH+capH*.2f, h3=stalkH;
        auto pt=[](float a,float r,float h){ return glm::vec3(r*cosf(a),h,r*sinf(a)); };
        auto nm=[](glm::vec3 a,glm::vec3 b,glm::vec3 c){ return glm::normalize(glm::cross(b-a,c-a)); };

        glm::vec3 p00=pt(a0,r0,h0),p10=pt(a1,r0,h0),p01=pt(a0,r1,h1),p11=pt(a1,r1,h1);
        push(p00,nm(p00,p10,p11),{(float)i/SEG,0.0f}); push(p10,nm(p00,p10,p11),{(float)(i+1)/SEG,0.0f}); push(p11,nm(p00,p10,p11),{(float)(i+1)/SEG,0.3f});
        push(p00,nm(p00,p11,p01),{(float)i/SEG,0.0f}); push(p11,nm(p00,p11,p01),{(float)(i+1)/SEG,0.3f}); push(p01,nm(p00,p11,p01),{(float)i/SEG,0.3f});

        glm::vec3 p02=pt(a0,r2,h2),p12=pt(a1,r2,h2);
        push(p01,nm(p01,p11,p12),{(float)i/SEG,0.3f}); push(p11,nm(p01,p11,p12),{(float)(i+1)/SEG,0.3f}); push(p12,nm(p01,p11,p12),{(float)(i+1)/SEG,0.65f});
        push(p01,nm(p01,p12,p02),{(float)i/SEG,0.3f}); push(p12,nm(p01,p12,p02),{(float)(i+1)/SEG,0.65f}); push(p02,nm(p01,p12,p02),{(float)i/SEG,0.65f});

        glm::vec3 p03=pt(a0,r3,h3),p13=pt(a1,r3,h3);
        push(p02,nm(p02,p12,p13),{(float)i/SEG,0.65f}); push(p12,nm(p02,p12,p13),{(float)(i+1)/SEG,0.65f}); push(p13,nm(p02,p12,p13),{(float)(i+1)/SEG,1.0f});
        push(p02,nm(p02,p13,p03),{(float)i/SEG,0.65f}); push(p13,nm(p02,p13,p03),{(float)(i+1)/SEG,1.0f}); push(p03,nm(p02,p13,p03),{(float)i/SEG,1.0f});

        push(p13,nm(p13,p03,p00),{(float)(i+1)/SEG,1.f}); push(p03,nm(p13,p03,p00),{(float)i/SEG,1.f}); push(glm::vec3(0,stalkH,0),{0,0,1},{.5f,.5f});
    }
    return v;
}

std::vector<float> buildPebble(float rx, float ry, float rz) {
    std::vector<float> v;
    const int LAT=8, LON=12;
    for (int i=0;i<LAT;i++) for (int j=0;j<LON;j++) {
        float t0=glm::pi<float>()*(float)i/LAT, t1=glm::pi<float>()*(float)(i+1)/LAT;
        float p0=glm::two_pi<float>()*(float)j/LON, p1=glm::two_pi<float>()*(float)(j+1)/LON;
        auto pt=[&](float t,float p){ return glm::vec3(rx*sinf(t)*cosf(p), ry*cosf(t), rz*sinf(t)*sinf(p)); };
        auto nm=[](glm::vec3 a,glm::vec3 b,glm::vec3 c){ return glm::normalize(glm::cross(b-a,c-a)); };
        glm::vec3 A=pt(t0,p0),B=pt(t0,p1),C=pt(t1,p0),D=pt(t1,p1);
        auto push2=[&](glm::vec3 a,glm::vec3 b,glm::vec3 cc){
            glm::vec3 n=nm(a,b,cc);
            for (auto p:{a,b,cc}){v.push_back(p.x);v.push_back(p.y);v.push_back(p.z);v.push_back(n.x);v.push_back(n.y);v.push_back(n.z);v.push_back(.5f);v.push_back(.5f);}
        };
        push2(A,B,D); push2(A,D,C);
    }
    return v;
}