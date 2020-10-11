
package com.tc.browser;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import com.tc.browser.facade.Client;


public class LoginDialog extends javax.swing.JDialog {

 
    public LoginDialog(java.awt.Frame parent, boolean modal) {
        super(parent, modal);
        initComponents();
    }
              
    private void initComponents() {

        user = new javax.swing.JTextField();
        userl = new javax.swing.JLabel();
        pwd = new javax.swing.JPasswordField();
        pwdl = new javax.swing.JLabel();
        login = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setName("Form"); // NOI18N


        userl.setText("ÓÃ»§"); // NOI18N
 


        pwdl.setText("ÃÜÂë"); // NOI18N
        login.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent arg0) {
				boolean r = Client.getInstance().login(user.getText(), pwd.getText());
				if(r)
					LoginDialog.this.setVisible(false);
				
			}
        	
        });
        login.setText("µÇÂ½"); // NOI18N


        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(107, 107, 107)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(userl)
                    .addComponent(pwdl))
                .addGap(35, 35, 35)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(login)
                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                        .addComponent(pwd, 0, 0, Short.MAX_VALUE)
                        .addComponent(user, javax.swing.GroupLayout.PREFERRED_SIZE, 122, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(112, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(87, 87, 87)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(user, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(userl))
                .addGap(26, 26, 26)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(pwd, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(pwdl))
                .addGap(39, 39, 39)
                .addComponent(login)
                .addContainerGap(83, Short.MAX_VALUE))
        );

       
        pack();
    }// </editor-fold>                        
          
    private javax.swing.JButton login;
    private javax.swing.JLabel userl;
    private javax.swing.JLabel pwdl;
    private javax.swing.JPasswordField pwd;
    private javax.swing.JTextField user;
    // End of variables declaration                   

}
