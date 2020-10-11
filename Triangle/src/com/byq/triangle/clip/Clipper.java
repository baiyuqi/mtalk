package com.byq.triangle.clip;
import java.util.*;
public class Clipper {

 public Clipper() {
 }

  public Clipper(Vector poly, Vector set) {
  bpoly = poly;
  cset = set;
 }
 Vector bpoly;
 Vector result;
 Vector cset;
}