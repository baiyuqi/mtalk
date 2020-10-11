package com.byq.triangle.triangles;

class InsertAlgorithm {
  QueryList qlist;
  VertexSet vlist;
  NetParam np;
  BasicOperation bop;
  public InsertAlgorithm(VertexSet vl,QueryList ql,NetParam npa,BasicOperation bo) {
  qlist=ql;
  vlist=vl;
  np=npa;
  bop=bo;
  }
  public void ReceiveVertex(Vertex vertex)
  {
    Trigon t=qlist.GetTrigon(vertex);
    if(t==null)return;
    vlist.InsertVertex(vertex);
    InsertMid(vertex,t);
    Queue.Process();
  }
  public void InsertToNet(Vertex vertex)
  {
    Trigon t=qlist.GetTrigon(vertex);
    if(t==null)return;
//    vlist.Insert(vertex);
    InsertMid(vertex,t);
    Queue.Process();
//    np.nov++;
  }
  public void PrepareVertex(Vertex vertex)
  {
//    vlist.Insert(vertex);
    vlist.InsertVertex(vertex);
//    np.nov++;
  }
  public void InsertMid(Vertex v,Trigon t)
  {
  	Trigon[] pT=new Trigon[3],p=new Trigon[3];
  	byte[] vid=new byte[3];
  	for(byte i=0;i<3;i++)
  	{
  		pT[i]=new Trigon();
  		p[i]=t.nbtri[i];
  		vid[i]=t.nbvex[i];
  	}
  	for(byte i=0;i<3;i++)
  	{
  		for(byte j=0;j<3;j++)
  		{
  			if(i!=j)
  			{
    			pT[i].nbvex[j]=i;
    			pT[i].nbtri[j]=pT[j];
    			pT[i].vex[j]=t.vex[j];
  			}
  			else
  			{
    			pT[i].nbvex[j]=vid[i];
    			pT[i].nbtri[j]=p[j];
    			pT[i].vex[j]=v;
  			}
  		}
  	}
    bop.DeleteTrigon(t);
  	for( byte i=0;i<3;i++)
  		bop.InsertTrigon(pT[i]);

  	QueueNode[] edge=new QueueNode[3];
  	for(byte i=0;i<3;i++)
  	{
			edge[i]=new QueueNode();
			pT[i].etc[i]=edge[i];
			edge[i].trigon=pT[i];
			edge[i].vid=i;
			Queue.Enter(edge[i]);
  	}
  }
  public void ChangeEdge(QueueNode edge)
  {
 	/*********************************************************************************
	 *边调整实际上是删去两个三角片又加入两个三角片的过程，另外有两点应注意：edge已经出了优 *
	 *化队列，不必有删队列的动作；判断周边相对的三角是否为空，以决定是否将该边入优化队列。 *
	 *********************************************************************************/
    Trigon p1=edge.trigon;
  	byte vid=edge.vid;
  	Trigon p2=p1.nbtri[vid];
  	int vid1=p1.nbvex[vid];
  	if(!(p1!=null&&p2!=null))
  		return;

  	Trigon new1=new Trigon();
  	Trigon new2=new Trigon();

  	new1.vex[0]=p1.vex[vid];
  	new1.vex[1]=p1.vex[(vid+1)%3];
  	new1.vex[2]=p2.vex[vid1];
  	new1.nbtri[0]=p2.nbtri[(vid1+1)%3];
  	new1.nbtri[1]=new2;
  	new1.nbtri[2]=p1.nbtri[(vid+2)%3];
  	new1.nbvex[0]=p2.nbvex[(vid1+1)%3];
  	new1.nbvex[1]=1;
  	new1.nbvex[2]=p1.nbvex[(vid+2)%3];

  	new2.vex[0]=p2.vex[vid1];
  	new2.vex[1]=p2.vex[(vid1+1)%3];
  	new2.vex[2]=p1.vex[vid];
  	new2.nbtri[0]=p1.nbtri[(vid+1)%3];
  	new2.nbtri[1]=new1;
  	new2.nbtri[2]=p2.nbtri[(vid1+2)%3];
  	new2.nbvex[0]=p1.nbvex[(vid+1)%3];
  	new2.nbvex[1]=1;
  	new2.nbvex[2]=p2.nbvex[(vid1+2)%3];

    bop.DeleteTrigon(p1);
    bop.DeleteTrigon(p2);

  	bop.InsertTrigon(new1);
  	bop.InsertTrigon(new2);

  	if(new1.nbtri[0]!=null)
  	{
  		QueueNode edge1=new QueueNode();
  		new1.etc[0]=edge1;
  		edge1.trigon=new1;
  		edge1.vid=0;
   		Queue.Enter(edge1);
    }

  	if(new1.nbtri[2]!=null){
  		QueueNode edge2=new QueueNode();
  		new1.etc[2]=edge2;
  		edge2.trigon=new1;
  		edge2.vid=2;
   		Queue.Enter(edge2);
   	}
  	if(new2.nbtri[0]!=null)
  	{
      QueueNode edge3=new QueueNode();
  		new2.etc[0]=edge3;
  		edge3.trigon=new2;
  		edge3.vid=0;
  		Queue.Enter(edge3);
  	}

  	if(new2.nbtri[2]!=null)
  	{
   		QueueNode edge4=new QueueNode();
  		new2.etc[2]=edge4;
  		edge4.trigon=new2;
  		edge4.vid=2;
  		Queue.Enter(edge4);
    }
  }
}