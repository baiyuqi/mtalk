package com.tc.browser;

import java.awt.AlphaComposite;
import java.awt.Component;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragSource;
import java.awt.dnd.DragSourceDragEvent;
import java.awt.dnd.DragSourceDropEvent;
import java.awt.dnd.DragSourceEvent;
import java.awt.dnd.DragSourceListener;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetDragEvent;
import java.awt.dnd.DropTargetDropEvent;
import java.awt.dnd.DropTargetEvent;
import java.awt.dnd.DropTargetListener;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;

import javax.swing.Icon;
import javax.swing.JLabel;
import javax.swing.JTree;
import javax.swing.Timer;
import javax.swing.event.TreeExpansionEvent;
import javax.swing.event.TreeExpansionListener;
import javax.swing.filechooser.FileSystemView;

import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

import org.apache.commons.httpclient.HttpException;

import com.tc.browser.node.FileNode;

public class DragTree extends JTree implements DragGestureListener,
        DragSourceListener, DropTargetListener {
    BufferedImage ghostImage;

    private Rectangle2D ghostRect = new Rectangle2D.Float();

    private Point ptOffset = new Point();

    private Point lastPoint = new Point();

    private TreePath lastPath;

    private Timer hoverTimer;

    FileNode sourceNode;

    public DragTree(FileNode fn) {
    	this.sourceNode = fn;
        DragSource dragSource = DragSource.getDefaultDragSource();

        dragSource.createDefaultDragGestureRecognizer(this, // component where
                // drag originates
                DnDConstants.ACTION_COPY_OR_MOVE, // actions
                this); // drag gesture recognizer
        setModel(createTreeModel());

        addTreeExpansionListener(new TreeExpansionListener() {
            public void treeCollapsed(TreeExpansionEvent e) {
            }

            public void treeExpanded(TreeExpansionEvent e) {
                TreePath path = e.getPath();

                if (path != null) {
                    FileNode node = (FileNode) path.getLastPathComponent();

                    if (!node.isExplored()) {
                        DefaultTreeModel model = (DefaultTreeModel) getModel();
                        try {
							node.explore();
						} catch (HttpException e1) {
							// TODO Auto-generated catch block
							e1.printStackTrace();
						} catch (IOException e1) {
							// TODO Auto-generated catch block
							e1.printStackTrace();
						}
                        model.nodeStructureChanged(node);
                    }
                }
            }
        });
        this.setCellRenderer(new DefaultTreeCellRenderer() {

            public Component getTreeCellRendererComponent(JTree tree,
                    Object value, boolean selected, boolean expanded,
                    boolean leaf, int row, boolean hasFocus) {
                TreePath tp = tree.getPathForRow(row);
                if (tp != null) {
                    /*FileNode node = (FileNode) tp.getLastPathComponent();
                    String p = node.getFilePath();
                    if(p != null){
                    File f = new File(p);
                    try {
                        Icon icon = FileSystemView.getFileSystemView().getSystemIcon(f);
                        this.setIcon(icon);
                        this.setLeafIcon(icon);
                        this.setOpenIcon(icon);
                        this.setClosedIcon(icon);
                        this.setDisabledIcon(icon);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    }*/
                }
                return super.getTreeCellRendererComponent(tree, value,
                        selected, expanded, leaf, row, hasFocus);
            }

        });

        super.setScrollsOnExpand(true);
        new DropTarget(this, DnDConstants.ACTION_COPY_OR_MOVE, this);

        //      Set up a hover timer, so that a node will be automatically expanded
        // or collapsed
        // if the user lingers on it for more than a short time
        hoverTimer = new Timer(1000, new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if (lastPath == null) {
                    return;
                }
                if (getRowForPath(lastPath) == 0)
                    return; // Do nothing if we are hovering over the root node
                if (isExpanded(lastPath))
                    collapsePath(lastPath);
                else
                    expandPath(lastPath);
            }
        });
        hoverTimer.setRepeats(false); // Set timer to one-shot mode

        this.addKeyListener(new KeyAdapter() {

            public void keyPressed(KeyEvent e) {
                int code = e.getKeyCode();
                int modifiers = e.getModifiers();
                if (code== 'v' || code == 'V') {
                    System.out.println("find v");
                    System.out.println("modifiers:" + modifiers + "\t"
                            + ((modifiers & KeyEvent.CTRL_MASK) != 0));
                }

                if ((modifiers & KeyEvent.CTRL_MASK) != 0
                        && (code == 'v' || code == 'V')) {
                    Transferable tr = Toolkit.getDefaultToolkit()
                            .getSystemClipboard().getContents(null);

                    TreePath path = getSelectionPath();
                    if (path == null) {
                        return;
                    }
                    FileNode node = (FileNode) path.getLastPathComponent();
                    if (node.isDirectory()) {
                        System.out.println("file cp");
                        try {
                            List list = (List) (tr
                                    .getTransferData(DataFlavor.javaFileListFlavor));
                            Iterator iterator = list.iterator();
                            
                            while (iterator.hasNext()) {
                                File f = (File) iterator.next();
                                cp(f.getPath(),node.getFilePath());
                            }
                            node.reexplore();
                        } catch (Exception ioe) {
                            ioe.printStackTrace();
                        }
                        updateUI();
                    }
                }
            }

        });
    }

    public void dragGestureRecognized(DragGestureEvent e) {
        // drag anything ��

        TreePath path = getLeadSelectionPath();
        if (path == null)
            return;
        FileNode node = (FileNode) path.getLastPathComponent();
        sourceNode = node;
        //      Work out the offset of the drag point from the TreePath bounding
        // rectangle origin
        Rectangle raPath = getPathBounds(path);
        Point ptDragOrigin = e.getDragOrigin();
        ptOffset.setLocation(ptDragOrigin.x - raPath.x, ptDragOrigin.y
                - raPath.y);
        //      Get the cell renderer (which is a JLabel) for the path being dragged
        int row = this.getRowForLocation(ptDragOrigin.x, ptDragOrigin.y);
        JLabel lbl = (JLabel) getCellRenderer().getTreeCellRendererComponent(
                this, // tree
                path.getLastPathComponent(), // value
                false, // isSelected (dont want a colored background)
                isExpanded(path), // isExpanded
                getModel().isLeaf(path.getLastPathComponent()), // isLeaf
                row, // row (not important for rendering)
                false // hasFocus (dont want a focus rectangle)
                );
        lbl.setSize((int) raPath.getWidth(), (int) raPath.getHeight()); // <�C
        // The
        // layout
        // manager
        // would
        // normally
        // do
        // this

        // Get a buffered image of the selection for dragging a ghost image
        this.ghostImage = new BufferedImage((int) raPath.getWidth(),
                (int) raPath.getHeight(), BufferedImage.TYPE_INT_ARGB_PRE);
        Graphics2D g2 = ghostImage.createGraphics();

        // Ask the cell renderer to paint itself into the BufferedImage
        g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC, 0.5f));
        // Make the image ghostlike
        lbl.paint(g2);

        g2.dispose();
        //this.getGraphics().drawImage(ghostImage, e.getDragOrigin().x,
        //        e.getDragOrigin().y, this);

        e.startDrag(null, // cursor
                ghostImage, new Point(5, 5),
                new StringSelection(getFilename()), // transferable
                this); // drag source listener
    }

    public void dragDropEnd(DragSourceDropEvent e) {
        ghostImage = null;
        sourceNode = null;
    }

    public void dragEnter(DragSourceDragEvent e) {
    }

    public void dragExit(DragSourceEvent e) {
        if (!DragSource.isDragImageSupported()) {
            repaint(ghostRect.getBounds());
        }
    }

    public void dragOver(DragSourceDragEvent e) {

    }

    public void dropActionChanged(DragSourceDragEvent e) {
    }

    public String getFilename() {
        TreePath path = getLeadSelectionPath();
        FileNode node = (FileNode) path.getLastPathComponent();
        return node.getFilePath();
    }

    private DefaultTreeModel createTreeModel() {

        return new DefaultTreeModel(sourceNode);
    }

    public void dragEnter(DropTargetDragEvent dtde) {

    }

    public void dragOver(DropTargetDragEvent dtde) {

        Point pt = dtde.getLocation();
        if (pt.equals(lastPoint)) {
            return;
        }
        if (ghostImage != null) {
            Graphics2D g2 = (Graphics2D) getGraphics();
            // If a drag image is not supported by the platform, then draw my
            // own drag image
            if (!DragSource.isDragImageSupported()) {
                paintImmediately(ghostRect.getBounds()); // Rub out the last
                // ghost image and cue
                // line
                // And remember where we are about to draw the new ghost image
                ghostRect.setRect(pt.x - ptOffset.x, pt.y - ptOffset.y,
                        ghostImage.getWidth(), ghostImage.getHeight());
                g2.drawImage((ghostImage), AffineTransform
                        .getTranslateInstance(ghostRect.getX(), ghostRect
                                .getY()), null);
            }
        }
        TreePath path = getClosestPathForLocation(pt.x, pt.y);
        if (!(path == lastPath)) {
            lastPath = path;
            hoverTimer.restart();
        }
    }

    public void dropActionChanged(DropTargetDragEvent dtde) {

    }

    public void drop(DropTargetDropEvent e) {
        try {
            DataFlavor stringFlavor = DataFlavor.stringFlavor;
            Transferable tr = e.getTransferable();

            TreePath path = this.getPathForLocation(e.getLocation().x, e
                    .getLocation().y);
            if (path == null) {
                e.rejectDrop();
                return;
            }
            FileNode node = (FileNode) path.getLastPathComponent();
            if (e.isDataFlavorSupported(DataFlavor.javaFileListFlavor)
                    && node.isDirectory()) {
                e.acceptDrop(DnDConstants.ACTION_COPY_OR_MOVE);
                System.out.println("file cp");
                List list = (List) (e.getTransferable()
                        .getTransferData(DataFlavor.javaFileListFlavor));
                Iterator iterator = list.iterator();
                //File parent = node.getFile();
                while (iterator.hasNext()) {
                    File f = (File) iterator.next();
                    cp(f.getPath(), node.getFilePath());
                }
                node.reexplore();
                e.dropComplete(true);
                this.updateUI();
            } else if (e.isDataFlavorSupported(stringFlavor)
                    && node.isDirectory()) {
                String filename = (String) tr.getTransferData(stringFlavor);
                if (filename.endsWith(".txt") || filename.endsWith(".java")
                        || filename.endsWith(".jsp")
                        || filename.endsWith(".html")
                        || filename.endsWith(".htm")) {
                    e.acceptDrop(DnDConstants.ACTION_COPY_OR_MOVE);
                    File f = new File(filename);
                    if (f.exists()) {
                        //f.renameTo(new File(node.getFile(), f.getName()));
                        node.reexplore();
                        ((FileNode) sourceNode.getParent()).remove(sourceNode);
                        e.dropComplete(true);
                        this.updateUI();
                    } else {
                        e.rejectDrop();
                    }
                } else {
                    e.rejectDrop();
                }
            } else {
                e.rejectDrop();
            }
        } catch (IOException ioe) {
            ioe.printStackTrace();
        } catch (UnsupportedFlavorException ufe) {
            ufe.printStackTrace();
        } finally {
            ghostImage = null;
            this.repaint();
        }
    }

    private void cp(String string, String string2) throws IOException {
       System.out.println("copy!!!");
    }

    public void dragExit(DropTargetEvent dte) {

    }
}