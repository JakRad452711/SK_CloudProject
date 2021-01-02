package testowyframes;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;

public class GUI implements ActionListener{
	private static JButton button_1;
	private static JButton button_2;
	private static JLabel userLabel;
	private static JFrame frame;
	private static JPanel panel;
	private static JTextField userText;
	private static JLabel passwordLabel;
	public String welcomeUserText;
	private static JPanel panelSuccses;
	private static JLabel afterLoginUser;
	private static JLabel filesText;
	

	public GUI () 	
	{
	
	}


	public static void main(String[] args) {
		
		
		
		
		panel = new JPanel();
		frame = new JFrame();
		frame.setSize(500,500);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.add(panel);
		

		panel.setLayout(null);
		
		userLabel = new JLabel ("U¿ytkownik");
		userLabel.setBounds (10,20,80,25);
		panel.add(userLabel);
		
		
		userText = new  JTextField (30);
		userText.setBounds (100,20,165,25);
		panel.add(userText);
		
		passwordLabel = new JLabel ("Has³o");
		passwordLabel.setBounds (10,50,80,25);
		panel.add(passwordLabel);
		
		JPasswordField passwordText = new JPasswordField ();
		passwordText.setBounds(100,50,165,25);
		panel.add(passwordText);
		
		button_1 = new JButton("Zaloguj");
		button_1.setBounds(10,80,130,25);
		button_1.addActionListener (new GUI());
		panel.add(button_1);
		
		button_2 = new JButton ("Wejdz jako goœæ");
		button_2.setBounds(180,80,130,25);
		button_2.addActionListener (new GUI());
		panel.add(button_2);
		
		frame.setVisible(true);
		
		
		
	}



	@Override
	public void actionPerformed(ActionEvent e) 
		{
		while(true)
		{
			if (e.getSource() == button_1)
			{
			System.out.print("test1");
			welcomeUserText = userText.getText();
			//miejsce na weryfikacje logowania 
			// if (flagaLogin == 0 )
			//{
			// komunikat o b³êdzie logowania
			// }
			break;
			}
			else if (e.getSource() == button_2 ) 
			{
				System.out.print("test2");
				welcomeUserText = "Goœæ";
				break;
			}
		}	
			frame.getContentPane().removeAll();
			
			panelSuccses = new JPanel();
			panelSuccses.setLayout(null);
			
			frame.add(panelSuccses);
			
			afterLoginUser = new JLabel ("Witaj " + welcomeUserText);
			afterLoginUser.setBounds(10, 20 , 80, 25);
			
			filesText = new JLabel ("Oto pliki na serwerze");
			
			
			panelSuccses.add(afterLoginUser);
			panelSuccses.add(filesText);
			
		
			
			
			frame.repaint();
			frame.setVisible(true);
		
			
			
			
			
			
		}
	}



	



	

	
