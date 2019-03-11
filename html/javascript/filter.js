/**
 *
 * @licstart  The following is the entire license notice for the 
 *  JavaScript code in this page.
 *
 * Copyright (C) 2015  Andrew Rogers
 *
 *
 * The JavaScript code in this page is free software: you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License (GNU GPL) as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.  The code is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.
 *
 * As additional permission under GNU GPL version 3 section 7, you
 * may distribute non-source (e.g., minimized or compacted) forms of
 * that code without the copy of the GNU GPL normally required by
 * section 4, provided you include this license notice and a URL
 * through which recipients can access the Corresponding Source.
 *
 * @licend  The above is the entire license notice
 * for the JavaScript code in this page.
 *
 */

// IIR filters are normally implemented as a series
// concatenation of biquadratic filters.

// x[n] ---->(+)-------o--->--->(+)---> y[n]
//            ^        |   b0    ^
//            |       [ ]        |
//            |        |         |
//           (+)<--<---o--->--->(+)
//            ^   -a1  |   b1    ^
//            |       [ ]        |
//            |        |         |
//            '----<---o--->-----'
//                -a2      b2

var Filter=function() {
    this.zeros=[];
    this.poles=[];
    this.gain=1;
};

Filter.prototype.elliptic_lpf=function() {

  // Zeros on unit-circle
  var za0=1.6877;
  var za1=2.2297;
  this.zeros=[expi(za0), expi(za1)];

  // Poles
  var pr0=0.94984;
  var pa0=1.5759;
  var pr1=0.57271;
  var pa1=1.2626;
  var p0=cmul([pr0,0],expi(pa0));
  var p1=cmul([pr1,0],expi(pa1));
  this.poles=[p0 ,p1];
};

Filter.prototype.getPolar=function(bq_id) {
  var z=this.zeros[bq_id];
  var zr=Math.sqrt(z[0]*z[0]+z[1]*z[1]);
  var za=Math.atan2(z[1],z[0]);
  var p=this.poles[bq_id];
  var pr=Math.sqrt(p[0]*p[0]+p[1]*p[1]);
  var pa=Math.atan2(p[1],p[0]);
  return [[zr,za],[pr,pa]];
}

Filter.prototype.setZeroFromPolar=function(bq_id,zp) {
  this.zeros[bq_id]=rcmul(zp[0],expi(zp[1]));
}

Filter.prototype.setPoleFromPolar=function(bq_id,pp) {
  this.poles[bq_id]=rcmul(pp[0],expi(pp[1]));
}

Filter.prototype.getZP=function() {
  // Insert conjugates for complex zeros
  var z=[];
  for( var n=0; n<this.zeros.length; n++){
    var zn=this.zeros[n];
    if(zn.constructor===Array){ // Complex
      z.push(zn);
      z.push(conj(zn));
    }
    else z.push([zn,0]); // Real
  }
  
  // Insert conjugates for complex poles
  var p=[];
  for( var n=0; n<this.poles.length; n++){
    var pn=this.poles[n];
    if(pn.constructor===Array){ // Complex
      p.push(pn);
      p.push(conj(pn));
    }
    else p.push([pn,0]); // Real
  } 

  return [z,p];
};

// z and p are the zeros and poles respectively.
// z and p are arrays of complex or real numbers,
// complex numbers are assumed to have a conjugate,
// these conjugates should not be included in z or p.
Filter.prototype.getBQ = function()
{
	var l=this.zeros.length;
	if(this.poles.length>l)l=this.poles.length;
	var y=[[1,0,0,1,0,0]];
	y.length=l;
	for( var i=0; i<l; i++){

		var zpoly1=0;
		var zpoly2=0;
		if(this.zeros[i]){
			var zi=this.zeros[i];
			if(zi.constructor===Array){
				zpoly1=-2*zi[0];
				zpoly2=zi[0]*zi[0]+zi[1]*zi[1];
			}
			else{
				zpoly1=-zi;
			}
		}

		var ppoly1=0;
		var ppoly2=0;
		if(this.poles[i]){
			var pi=this.poles[i];
			if(pi.constructor===Array){
				ppoly1=-2*pi[0];
				ppoly2=pi[0]*pi[0]+pi[1]*pi[1];
			}
			else{
				ppoly1=-pi;
			}
		}

		y[i]=[1, zpoly1, zpoly2, 1, ppoly1, ppoly2];
	}
	return y;
};

Filter.prototype.getFreqResponse = function()
{
    var bq=this.getBQ();
	var n=512;
	var h=[];
        var gain=0.001;
	h.length=n;
	for( var f=0; f<n; f++){
		var z=expi(Math.PI*f/n);
		var z2=[z[0]*z[0]-z[1]*z[1], 2*z[0]*z[1]];
		var ht=1;
		for( var m=0; m<bq.length; m++ ){

			// Numerator quadratic
			var v0=rcmul(bq[m][0],z2); // b0*z^2
			var v1=rcmul(bq[m][1],z);  // b1*z
			var v2=bq[m][2];           // b2

			// Denominator quadratic
			var v3=rcmul(bq[m][3],z2); // a0*z^2
			var v4=rcmul(bq[m][4],z);  // a1*z
			var v5=bq[m][5];           // a2

			ht=ht*cdivabs([v0[0]+v1[0]+v2, v0[1]+v1[1]], [v3[0]+v4[0]+v5, v3[1]+v4[1]]);
		}
                if( ht>gain ) gain=ht;
		h[f]=[f/n, ht];
	}
        for( var f=0; f<n; f++) h[f][1]=h[f][1]/gain;
        this.gain=gain;
	return(h);
};
