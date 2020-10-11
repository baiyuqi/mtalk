package com.byq.triangle.triangles;

public class ListParam {
  double startx,starty,endx,endy,cellsize;
  int nx,ny;
  public ListParam(RawDataInfo DataInfo) {
    startx=DataInfo.minx-(DataInfo.maxx-DataInfo.minx)*0.05;
    starty=DataInfo.miny-(DataInfo.maxy-DataInfo.miny)*0.05;
    endx=DataInfo.maxx+(DataInfo.maxx-DataInfo.minx)*0.05;
    endy=DataInfo.maxy+(DataInfo.maxy-DataInfo.miny)*0.05;
    cellsize=(DataInfo.maxx-DataInfo.minx)/Math.sqrt(DataInfo.pnumber);
//    int nx,ny;
    nx=(int)((endx - startx)/cellsize+1) ;
    ny=(int)((endy - starty)/cellsize+1) ;
    endx = startx + nx * cellsize;
    endy = starty + ny * cellsize;
  }
}