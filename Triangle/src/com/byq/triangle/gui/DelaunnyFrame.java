package com.byq.triangle.gui;

import java.awt.AWTEvent;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FocusTraversalPolicy;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.WindowEvent;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JToggleButton;

import com.byq.triangle.creutil.MapWindowTranslator;
import com.byq.triangle.matrixcon.Mesh;
import com.byq.triangle.render.AreasetRenderer;
import com.byq.triangle.render.ContourRenderer;
import com.byq.triangle.render.DelaunnyRenderType;
import com.byq.triangle.render.DelaunnyRenderer;
import com.byq.triangle.render.ImageRenderer;
import com.byq.triangle.render.Renderer;
import com.byq.triangle.session.NetSession;

public class DelaunnyFrame extends JFrame {

	RenderPanel renderPanel;
	JPanel tools;

	JButton inputMode;
	JButton createNet;
	JButton createMesh;
	 
	JToggleButton[] renderMode = new JToggleButton[8];

	ButtonGroup group;
	MeshFrm meshfrm;


	String[] modeDesc = new String[] { "输入点集", "输入边界", "插值矩形" ,"点选范围"};
	String[] renderDesc = new String[] { "D", "V", "DV" ,"C","SC","I", "SI", "A"};

	NetSession session;
	public DelaunnyFrame() {
		enableEvents(AWTEvent.WINDOW_EVENT_MASK);
		try {

			
			session = new NetSession();
			tools = new JPanel();
			tools.setPreferredSize(new Dimension(100, 30));
			tools.setBorder(BorderFactory.createEtchedBorder());
			tools.setLayout(null);
			renderPanel = new RenderPanel(session);
			renderPanel.setFocusable(true);
//			renderPanel.setFocusTraversalPolicy(FocusTraversalPolicy.);
			renderPanel.setRequestFocusEnabled(true);
			this.getContentPane().setLayout(new BorderLayout());
			this.getContentPane().add(tools, BorderLayout.NORTH);
			this.getContentPane().add(renderPanel, BorderLayout.CENTER);
			
			
			inputMode = new JButton("输入点集");
			createMesh = new JButton("生成插值");
			createNet = new JButton("\u4ea7\u751f\u683c\u7f51");
			

			createNet.setBounds(90, 0, 90, 27);
			inputMode.setBounds(0, 0, 90, 27);
			createMesh.setBounds(180, 0, 90, 27);
			
//			createNet.setFont(new Font());
			tools.add(createNet);
			tools.add(inputMode);
			tools.add(createMesh);

			
			group = new ButtonGroup();
			for(int i = 0; i < renderMode.length; i++){
				renderMode[i] = new JToggleButton(renderDesc[i]);
				renderMode[i].setBounds(240 + 60 * i, 0, 90, 27);
				
				tools.add(renderMode[i]);
				group.add(renderMode[i]);
				final int k = i;
			renderMode[i].addActionListener(new ActionListener(){
					
				public void actionPerformed(ActionEvent e) {
					renderPanel.renderMode(k);
				}
			});
			}
			createNet.addActionListener(new ActionListener() {

					public void actionPerformed(ActionEvent e) {
						session.createNet();
						renderPanel.update(renderPanel.getGraphics());
					}
				});
			inputMode.addActionListener(new ActionListener() {

				public void actionPerformed(ActionEvent e) {
					int imode = renderPanel.nextInputMode();
					inputMode.setText(modeDesc[imode]);
				}
			});
			createMesh.addActionListener(new ActionListener() {

				public void actionPerformed(ActionEvent e) {
					createMeshFrm();
				}
			});

			this.setForeground(Color.gray);
			this.setTitle("Voronoi图");


		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	protected void processWindowEvent(WindowEvent e) {
		super.processWindowEvent(e);
		if (e.getID() == WindowEvent.WINDOW_CLOSING) {
			System.exit(0);
		}
	}

	public static void main(String[] args) {
		DelaunnyFrame myFrm1 = new DelaunnyFrame();

		myFrm1.setSize(1280, 768);
		myFrm1.reset();
		myFrm1.setVisible(true);
	}




	public void redraw() {
		Graphics g = this.getGraphics();
		this.update(g);
	}





	public void reset() {
		renderPanel.reset();

	}

	GridBagLayout gridBagLayout1 = new GridBagLayout();


	void createMeshFrm() {

		Mesh mesh = session.createMesh();
		if (meshfrm == null) {
			meshfrm = new MeshFrm(mesh, session.getBoundPoly());
			meshfrm.setSize(400, 400);
		} else
			meshfrm.setMesh(mesh);
		meshfrm.setVisible(true);
		meshfrm.reset();

	}


}
