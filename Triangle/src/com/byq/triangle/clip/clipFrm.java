package com.byq.triangle.clip;

import java.awt.*;
import javax.swing.JFrame;

import com.byq.triangle.geomath.*;
import com.byq.triangle.triangles.*;
/**
 * Title:        Your Product Name
 * Description:  Your description
 * Copyright:    Copyright (c) 1999
 * Company:      creation
 * @author byq
 * @version
 */
import java.awt.event.*;
import java.util.*;
public class clipFrm extends JFrame {
 Vector poly = new Vector();
 Area area;
 AreaSet aset;
 Vector line = new Vector();
 Vector lset = new Vector();
 Vector clset = new Vector();
 Vector result = new Vector();
 int state = 0;
 int dstate = 0;
 public clipFrm() {
  try {
   jbInit();
  }
  catch(Exception e) {
   e.printStackTrace();
  }
 }
   protected void processWindowEvent(WindowEvent e) {
    super.processWindowEvent(e);
    if(e.getID() == WindowEvent.WINDOW_CLOSING) {
      System.exit(0);
    }
  }
  public static void main(String[] args) {
    clipFrm myFrm1 = new clipFrm();
    myFrm1.setSize(600,400);
   // myFrm1.reset();
    myFrm1.show();
  }
 private void jbInit() throws Exception {
  this.addKeyListener(new java.awt.event.KeyAdapter() {
   public void keyPressed(KeyEvent e) {
    this_keyPressed(e);
   }
  });
  this.addMouseListener(new java.awt.event.MouseAdapter() {
   public void mousePressed(MouseEvent e) {
    this_mousePressed(e);
   }
  });
 }
 public void paint(Graphics g)
 {
    super.paint(g);
    if(dstate!= 0)
    {
//   for(int i = 0;i< result.size();i++)
  // drawPoly(g,(Vector)result.elementAt(i), new Color(100,100, 100));
    //  drawPoly(g,(Vector)result.elementAt(dstate-1), new Color(0,240, 0));
//      aset.draw(g);
      return;
      }
   drawLine(g, line);
    drawPoly(g, poly,new Color(255,0,0));
    for(int i = 0; i< result.size();i++)
    drawPoly(g,(Vector)result.elementAt(i), new Color(0,140, 0));
    for(int i = 0; i< lset.size();i++)
    drawLine(g, (Vector)lset.elementAt(i));
    for(int i = 0; i< clset.size();i++)
    drawPoly(g, (Vector)clset.elementAt(i),new Color(255,0,0));
 }
 void drawPoly(Graphics g, Vector poly,Color c)
 {
    if(poly == null)return;
    g.setColor(c);
    for(int i = 0; i < poly.size(); i++)
    {
       GVector p1 = (GVector)poly.elementAt(i),
       p2 = (GVector)poly.elementAt((i+1)%poly.size());

       g.drawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);

    }
 }
  void drawLine(Graphics g, Vector poly)
 {
    if(poly == null)return;
    for(int i = 0; i < poly.size()-1; i++)
    {
       GVector p1 = (GVector)poly.elementAt(i),
       p2 = (GVector)poly.elementAt((i+1)%poly.size());

       g.drawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);

    }
 }

 void this_mousePressed(MouseEvent e) {

  Point p = e.getPoint();
  switch(state){
  case 0:
  poly.addElement(new GVector(p.getX(), p.getY(),0));
  break;
  case 1:
  line.addElement(new GVector(p.getX(), p.getY(),0));break;
  case 2:
//    line1.addElement(new GVector(p.getX(), p.getY(),0));break;
}
  this.update(this.getGraphics());

 }

 void this_keyPressed(KeyEvent e) {

 switch(e.getKeyChar()){
 case 'a':
 state = (state + 1)%3;
 break;
 case 's':

// result = GeoMath.cutPoly(poly, line);
 createPSet();
 //for(int i = 0;i<result.size();i++)

 break;
  case 'z':
 dstate++;break;//= (dstate+1)%(result.size()+1);break;
 case 'm':
 lset.addElement(line);
 line = new Vector();
 break;
 case 'n':
 clset.addElement(line);
 line = new Vector();
 break;
 }
 this.update(this.getGraphics());
 }
 void createPSet()
 {
    aset = new AreaSet(0.);
    area = new Area(poly, null, aset, 1);
    aset.add(area);

    int n = lset.size();
    for(int i = 0; i < n; i++)
    {
    aset.LineCut((Vector)lset.elementAt(i), false);
    }
    int m =  clset.size();
    for(int i = 0; i < m; i++)
    {
    aset.LineCut((Vector)clset.elementAt(i), true);
    }

 }
  Vector getResult(Vector pset, Vector line)
 {
     Vector result = new Vector();
     Vector temp = new Vector();// = GeoMath.cutPoly(poly, line);
     result = copy(pset);
     temp = copy(pset);
     for(int i = 0;i<temp.size();i++)
     {
       Vector pl = (Vector)temp.elementAt(i);
       if(GeoMath.Intersect(pl,line))
       {
          result.removeElement(pl);
          Vector re = GeoMath.cutPoly(pl, line, true);
          int k = re.size();
          for(int j = 0; j< k; j++)
             result.addElement(re.elementAt(j));
       }

     }
     return result;
 }
 Vector getSetSect(Vector pset, Vector lset)
 {
   Vector result = copy(pset);
   for(int i = 0; i < lset.size(); i++)
     result = getResult(result, (Vector)lset.elementAt(i));
   return result;
 }
 Vector copy(Vector v)
 {
 if(v == null)return null;
 Vector r = new Vector();
 int n = v.size();
 for(int i = 0; i < n; i++)
  r.addElement(v.elementAt(i));
 return r;
 }
}