#include <math.h>

float twoPointLength(float x1, float y1, float x2, float y2){
  return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}


void attitudeAngle(float angle){
  // zg  0x0000 ～ 0xffff --> -90°～ 90°
  float T = machineT;

  float R = angle * PI/180.0 - T;
  if(R > PI)  R = PI;
  if(R < -PI) R = -PI;
  R /= PI;

  float L = angle * PI/180.0 + T;
  if(L >PI)  L = PI;
  if(L < -PI) L = -PI;
  L /= PI;
  rightMoter  += R;
  leftMoter   += L;
  // Serial.printf("L:%.2f,R:%.2f\n",leftMoter,rightMoter);
}

void attitudeRad(float rad){
  // zg  0x0000 ～ 0xffff --> -90°～ 90°
  float dT = rad - machineT;
  if(dT > PI)  dT = PI;
  if(dT < -PI) dT = -PI;
  dT /= PI;

  if(dT > 0)      dT = easeOutSine(dT);
  else if(dT < 0) dT = -easeOutSine(-dT);

  // float max = 1.0 - dT;
  // if(max < 0.0) max = 0.0;
  
  float R = dT;
  float L = -dT;
  
  // if(leftMoter > max)  leftMoter = max;
  // if(leftMoter < -max) leftMoter =-max;
  // if(rightMoter > max)  rightMoter = max;
  // if(rightMoter < -max) rightMoter =-max;
  
  rightMoter  += R;
  leftMoter   += L;
  // Serial.printf("L:%.2f,R:%.2f\n",leftMoter,rightMoter);
}


struct PassingPoint* addPassingPoint(struct PassingPoint *startPoint, float x, float y, float ps, float ls){
  struct PassingPoint *pp = (PassingPoint *)malloc(sizeof(PassingPoint));
  startPoint->next = pp;
  pp->x = x;
  pp->y = y;
  pp->ps = ps;
  pp->ls = ls;
  return pp;
}

void printRoute(struct PassingPoint *route){
  printf("x:%.2f, y:%.2f, ps:%.2f, ls:%.2f\n",route->x,route->y,route->ps,route->ls);
  if(route->next) printRoute(route->next);
}

void runToPoint(struct PassingPoint *route){
  if(!route) return;
  float dis = twoPointLength(machineX,machineY,route->x,route->y);
  if(dis < 1.0) nextPoint = nextPoint->next;
  float theta = atan2(route->y - machineY,route->x - machineX);
  //if(theta * theta < 0.001) rightMoter = leftMoter = route->ps;
  rightMoter = leftMoter = route->ps;
  debugT = theta;
  attitudeRad(theta);
}

