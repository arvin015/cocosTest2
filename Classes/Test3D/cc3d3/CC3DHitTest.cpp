//#include "CC2D3Util.h"
#include "CC3DHitTest.h"
#include "LogUtil.h"

USING_NS_CC;

//http://stackoverflow.com/questions/9368436/3d-perpendicular-point-on-line-from-3d-point

float hittestlinepoint(const Vec3 &vorig, const Vec3 &vdir, const Vec3 &vp){

	Vec3 vn2c;//vector near point to corner point;
	vn2c = vp - vorig; //vectorSubtract(vp, vorig, vn2c);
    
	Vec3 vdist; // vp to proj point
	Vec3 vpnear; // vnear to proj point
	
    float dproj = vn2c.dot(vdir);//vectorDot(vn2c, vdir);
	float lensqdir = vdir.lengthSquared();//vectorLensq(vdir);
	vpnear = vdir * (dproj / lensqdir);  //vectorScale(vdir, dproj / lensqdir, vpnear);
	vdist = vn2c - vpnear; //vectorSubtract(vn2c, vpnear, vdist);
    
	return vdist.lengthSquared();// vectorLensq(vdist);
}


void hittestlinetri(const Vec3 &orig, const Vec3 &dir,
                    const Vec3 &vert0, const Vec3 &vert1, const Vec3 &vert2,
                    bool *o_hit, float *o_dist){
    *o_hit = false;
    
	Vec3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;
    float u, v;
    
	edge1 = vert1 - vert0;// vectorSubtract(vert1, vert0, edge1);
	edge2 = vert2 - vert0;// vectorSubtract(vert2, vert0, edge2);
    
	Vec3::cross(dir, edge2, &pvec); //vectorCrossProduct(dir, edge2, pvec);
	det = edge1.dot(pvec);// vectorDot(edge1, pvec);
    
    if(0.0f > det)
        return; //Cull Back Face
    
	tvec = orig - vert0;// vectorSubtract(orig, vert0, tvec); // tvec = distance v0 to orig
    
	u = tvec.dot(pvec);//vectorDot(tvec, pvec);
    if(u < 0.0f || u > det)
        return;
    
	Vec3::cross(tvec, edge1, &qvec);// vectorCrossProduct(tvec, edge1, qvec);
    
	v = dir.dot(qvec);//vectorDot(dir, qvec);
    if((v < 0.0) || ((u + v) > det))
        return;
    
	float t = edge2.dot(qvec);// vectorDot(edge2, qvec);
	inv_det = 1.0f / det;// invf(det);
    
    t *= inv_det;
    u *= inv_det;
    v *= inv_det;
    
    *o_hit = true;
    *o_dist = t;
}


void VecX(const Vec3 &p, const Vec3 &g, float s, Vec3 &x){
	x = p + (g * s);
	//x.x = p.x + s * g.x;
    //x.y = p.y + s * g.y;
    //x.z = p.z + s * g.z;
}


float DisA(const Vec3 &X1, const Vec3 &X2, Vec3 &Da){
	Da = X2 - X1;
	//Da.x = X2.x - X1.x;
    //Da.y = X2.y - X1.y;
    //Da.z = X2.z - X1.z;
	return Da.lengthSquared();//vectorLensq(Da);
}


// return distsq
float hittestlineline2(const Vec3 &p1, const Vec3 &q1,
                       const Vec3 &p2, const Vec3 &q2,
	Vec3 &x1, Vec3 &x2){
    //float dis;
    
    double a11, a12, a22, b1, b2, D0, D1, D2;
    double s1, s2, n1, n2, sa, sb, dq = -1.0, eq;
    int flg;
	Vec3 g1, g2, dp, da;
    
	g1 = q1 - p1; // vectorSubtract(q1, p1, g1);
	g2 = q2 - p2; // vectorSubtract(q2, p2, g2);
	dp = p2 - p1; // vectorSubtract(p2, p1, dp);
    
	a11 = g1.dot(g1);// vectorDot(g1, g1);
	a12 = -(g1.dot(g2));// -vectorDot(g1, g2);
	a22 = g2.dot(g2);// vectorDot(g2, g2);
    
	b1 = g1.dot(dp);// vectorDot(g1, dp);
	b2 = -(g2.dot(dp));// -vectorDot(g2, dp);
    
    //{ Cramer determinants }
    D0 = (a11 * a22) - (a12 * a12);
    D1 = (b1 * a22) - (b2 * a12);
    D2 = -(b1 * a12) + (b2 * a11);
    
    //{ In-line distance }
    flg = 0;
    if (D0 > 0) {
        if ( (0 < D1) && (D1 < D0) )
            flg++;
        if ( (0 < D2) && (D2 < D0) )
            flg++;
    }
    if (0 > D0) {
        if ( (0 > D1) && (D1 > D0) )
            flg++;
        if ( (0 > D2) && (D2 > D0) )
            flg++;
    }
    if (2 == flg) {
        sa = D1 / D0;
        sb = D2 / D0;
    }
    //{ Out-line distance }
    if( 2 > flg ){
        //{a11>=0, a22>=0 }
        s1 = 0;
        n2 = b2; //{ s2:=(b2-a12*s1)/a22 }
        flg = 0;
        if (n2 <= 0) {
            s2 = 0;
            flg++;
        }
        if (n2 >= a22) {
            s2 = 1;
            flg++;
        }
        if( 0 == flg )
            s2 = n2 / a22;
        
        VecX(p1, g1, s1, x1);
        VecX(p2, g2, s2, x2);
        eq = DisA(x1, x2, da);
        
        sa = s1;
        sb = s2;
        dq = eq;
        s1 = 1;
        n2 = b2 - a12; //{ s2:=(b2-a12*s1)/a22 }
        flg = 0;
        if ( n2 <= 0){
            s2 = 0;
            flg++;
        }
        if ( n2 >= a22){
            s2 = 1;
            flg++;
        }
        if ( 0 == flg )
            s2 = n2 / a22;
        
        VecX(p1, g1, s1, x1);
        VecX(p2, g2, s2, x2);
        eq = DisA(x1, x2, da);
        
        if (eq < dq) {
            sa = s1;
            sb = s2;
            dq = eq;
        }
        s2 = 0;
        n1 = b1; // { s1 = (b1-a12 * s2) / a11 }
        flg = 0;
        if ( n1 <= 0 ) {
            s1 = 0;
            flg++;
        }
        if ( n1 >= a11 ) {
            s1 = 1;
            flg++;
        }
        if( 0 == flg)
            s1 = n1 / a11;
        VecX(p1, g1, s1, x1);
        VecX(p2, g2, s2, x2);
        eq = DisA(x1, x2, da);
        if ( eq < dq ){
            sa = s1;
            sb = s2;
            dq = eq;
        }
        s2 = 1;
        n1 = b1 - a12; // { s1 = (b1-a12*s2)/a11 }
        flg = 0;
        if( n1 <= 0 ){
            s1 = 0;
            flg++;
        }
        if( n1 >= a11 ){
            s1= 1;
            flg++;
        }
        if( 0 == flg)
            s1 = n1 / a11;
        VecX(p1, g1, s1, x1);
        VecX(p2, g2, s2, x2);
        eq = DisA(x1, x2, da);
        if( eq < dq){
            sa = s1;
            sb = s2;
            dq = eq;
        }
    }
    //{ for all sa=s1, sb=s2, refresh }
    VecX(p1, g1, sa, x1);
    VecX(p2, g2, sb, x2);
    dq = DisA(x1, x2, da);
 
//    NSLog(@"hittestlineline2 Error (%f,%f,%f)~(%f,%f,%f) &\n(%f,%f,%f)~(%f,%f,%f):\nD0=%f, D1=%f, D2=%f",
//              p1.x, p1.y, p1.z, q1.x, q1.y, q1.z,
//              p2.x, p2.y, p2.z, q2.x, q2.y, q2.z,
//              D0, D1, D2);
    return dq;
}

//http://objectmix.com/graphics/133793-coordinates-closest-points-pair-skew-lines.html
float hittestlineline(const Vec3 &p1, const Vec3 &p1b,
                      const Vec3 &p2, const Vec3 &p2b,
	Vec3 &v){
    
	Vec3 g1, g2, r, d;
    float s1, s2;
    
	g1 = p1b - p1;// vectorSubtract(p1b, p1, g1); // g1 = dir of near to far
	g2 = p2b - p2;// vectorSubtract(p2b, p2, g2); // g2 = dir of edg1 ~ edg2
    
	r = p2 - p1;// vectorSubtract(p2, p1, r);
    
	float a11 = g1.dot(g1);// vectorDot(g1, g1);
	float a22 = g2.dot(g2);// vectorDot(g2, g2);
	float a12 = -(g1.dot(g2));// -vectorDot(g1, g2);
    
	float b1 = g1.dot(r);// vectorDot(g1, r);
	float b2 = -(g2.dot(r));// -vectorDot(g2, r);
    
    float D0 = a11 * a22 - a12 * a12;
    float D1 = b1 * a22 - b2 * a12;
    float D2 = b2 * a11 - b1 * a12;
    
    if (
        (( D0 > 0 ) && ( (D1 > 0 && D1 < D0) || (D2 > 0 && D2 < D0) )) ||
        (( D0 < 0 ) && ( (D1 < 0 && D1 > D0) || (D2 < 0 && D2 > D0) ))
        )
    {
        s1 = D1 / D0;
        s2 = D2 / D0;
        
		v = p1 + (g1 * s1);
        //v.x = p1.x + s1 * g1.x;
        //v.y = p1.y + s1 * g1.y;
        //v.z = p1.z + s1 * g1.z;
        
		d = r + (g2 * s2) + (g1 * -s1);
        //d.x = r.x + s2 * g2.x - s1 * g1.x;
        //d.y = r.y + s2 * g2.y - s1 * g1.y;
        //d.z = r.z + s2 * g2.z - s1 * g1.z;
        
		return d.lengthSquared();// vectorLensq(d);
    }
     
	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"hittestlineline Error (%f,%f,%f)~(%f,%f,%f) &\n(%f,%f,%f)~(%f,%f,%f):\nD0=%f, D1=%f, D2=%f",
		p1.x, p1.y, p1.z, p1b.x, p1b.y, p1b.z, //p1.x, p1.y, p1.z, p1b.x, p1b.y, p1b.z,
		p2.x, p2.y, p2.z, p2b.x, p2b.y, p2b.z, //p2.x, p2.y, p2.z, p2b.x, p2b.y, p2b.z,
		D0, D1, D2);
	return -1;
}


