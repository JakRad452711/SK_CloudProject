package testowyframes;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseListener;
import java.io.CharArrayReader;
import java.io.CharArrayWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Base64;
import java.util.Scanner;
import java.util.Set;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.JOptionPane;



public class GUI implements ActionListener , ListSelectionListener {
	
	private static JButton loginButton;
	private static JButton guestButton;
	private static JLabel userLabel;
	private static JFrame frame;
	private static JPanel panel;
	private static JTextField userText;
	private static JLabel passwordLabel;
	public String welcomeUserText;
	private static JPanel panelSuccess;
	private static JLabel afterLoginUser;
	private static JLabel filesText;
	private static 	JButton closeButton;
	public static  JList list;
	public String fileToDownload  ;
	private static JButton downloadButton;
	private static JLabel downloadTextLabel ;
	private static JButton uploadButton;
	private static JTextField uploadTextField;
	private static String fileNameToGive;
	private static  Process p ;
	private static JPasswordField passwordText;
	private static   Scanner myFileReader;
	private static  String files[];
	private static String passw;
	private static String toSendString;
	private static char[] toSendChar;
	private static byte[] sent;
	private static byte[] received;
	private static char[] response;
	private static PipeClass readerPipe;
	private static PipeClass writerPipe;
	private static String responseAnswer;
	private static String part1;
	private static String part2;
	private static String[] responseAnswerSplit;
	private static String userNameSendString;
	private static  byte [] bytesIntCommand ;
	
	public static void frameSetUp () 
	{
		
		panel = new JPanel();
		frame = new JFrame();
		frame.setSize(1000,700);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.add(panel);
	}
	
	public static void firstPanel () 	
	{

		panel.setLayout(null);
		
		userLabel = new JLabel ("Użytkownik");
		userLabel.setBounds (10,20,80,25);
		panel.add(userLabel);
		
		
		userText = new  JTextField (30);
		userText.setBounds (100,20,165,25);
		panel.add(userText);
		
		passwordLabel = new JLabel ("Hasło");
		passwordLabel.setBounds (10,50,80,25);
		panel.add(passwordLabel);
		
		passwordText = new JPasswordField ();
		passwordText.setBounds(100,50,165,25);
		panel.add(passwordText);
		
		loginButton = new JButton("Zaloguj");
		loginButton.setBounds(10,80,130,25);
		loginButton.addActionListener (new GUI());
		panel.add(loginButton);
		
		guestButton = new JButton ("Wejdz jako gość");
		guestButton.setBounds(180,80,130,25);
		guestButton.addActionListener (new GUI());
		panel.add(guestButton);
		closeButton = new JButton("Exit");
		closeButton.addActionListener(new CloseListener());
		closeButton.setBounds(300,300, 130,25);
		panel.add(closeButton);
		
		frame.setVisible(true);
		
		
	
	}
	
	public  void secondPanel () 
	{
	
		panelSuccess = new JPanel();
		panelSuccess.setLayout(null);
		
		frame.add(panelSuccess);
		
	
		
		
		closeButton = new JButton("Exit");
		closeButton.addActionListener(new GUI());
		closeButton.setBounds(300,500, 130,25);
		panelSuccess.add(closeButton);
		
		afterLoginUser = new JLabel ("Witaj, " + welcomeUserText);
		afterLoginUser.setBounds(10, 20 , 80, 25);
		
		filesText = new JLabel ("Oto pliki na serwerze");
		filesText.setBounds(10,60,260,25);
		
		panelSuccess.add(afterLoginUser);
		panelSuccess.add(filesText);
		
		
		File myObj = new File("nazwy_plikpw.txt");
	  
		try {
			myFileReader = new Scanner(myObj);
		} catch (FileNotFoundException e) {
			
			e.printStackTrace();
		}
	      
	
				 //while( input fifo)
		// string
		//TUTAJ DAĆ czytanie pliku READ I PRZYPISAC DO pliki STUFF
		ArrayList <String> filesNamesList = new ArrayList<>();; 
		while(myFileReader.hasNextLine()) 
		{
			filesNamesList.add(myFileReader.nextLine());
		}
		myFileReader.close();
		
		
		files = new String [filesNamesList.size()];
		int j=0;
		for (String z : filesNamesList) {
		    files[j] = z;
		    j++;
		}
		list = new JList(files);
		list.setBounds(50,150,150,6000);
		
		
		
		list.addListSelectionListener(new GUI()); 
		
		downloadButton = new JButton("Pobierz");
		downloadButton.setBounds(300,40,120,25);
		downloadButton.addActionListener(new GUI());
		
		downloadTextLabel = new JLabel ();
		downloadTextLabel.setText("Nazwa pliku"); 
		downloadTextLabel.setBounds(300,70,800,25);
		
		//dodanie przycisku z DODAJ PLIK
		uploadButton = new JButton ("Dodaj plik");
		uploadButton.setBounds(300,120,120 ,25);
		uploadButton.addActionListener(new GUI ());
		
		//text field na nazwe pliku
		uploadTextField = new JTextField();
		uploadTextField.setBounds(300,150,80,25);
		
		//fifo z pathname pliku do C
		//ewentualne odebranie komunikatu od C że podany plik nie istnieje
		panelSuccess.add(list);
		panelSuccess.add(downloadButton);
		panelSuccess.add(downloadTextLabel);
		panelSuccess.add(uploadTextField);
		panelSuccess.add(uploadButton);
	}
	

		
		
	
	
	public static void main(String[] args) throws IOException {
		///STWORENIE PLIKÓW DO FIFO
		
		readerPipe = new PipeClass("/usr/lib/CloudProjectNamedPipeCToJava");
		writerPipe = new PipeClass("/usr/lib/CloudProjectNamedPipeJavaToC");
		
		
		//stworzenie charów z formularza
		
		
		
		toSendString = "Guest\n123\nREQUEST_FILE_NAMES\n\n";
		
		toSendChar = toSendString.toCharArray();
		sent = new byte[4096];
		
		for(int i=0 ; i<toSendChar.length ; i++)
			sent[i] = (byte) toSendChar[i];
		
		writerPipe.write(sent);
		
		
		
		
	
		
		//obudzenie procesu C klient
		 p  = Runtime.getRuntime().exec("gnome-terminal --tab -- ./TCPClient.h") ;
		
		
		 	frameSetUp();
			firstPanel();
			
			
		
			
			
		
	}

	
	
	
	
	
	public void valueChanged(ListSelectionEvent e) 
    { 
        //set the text of the label to the selected value of lists 
        downloadTextLabel.setText("Nazwa pliku " + list.getSelectedValue()); 
         
       fileNameToGive = downloadTextLabel.getText();
        //if     fileToDownload.endsWith("/");
          // wyswietlam zawartosc katalogu	
        System.out.print(fileToDownload);
        fileNameToGive = fileToDownload;
        
    } 
	

	@Override
	public void actionPerformed(ActionEvent e) 
		{
	
		
			if (e.getSource() == loginButton)
			{	// wpierw write to do fifo tak samo jak wszędzie indziej a potem readera
				//Ten fragment kodu odpowiada za otworzenie pliku ACCEPT/DENIED, odczytanie jego 
				//musi powstac czytnik fifo który zrozumie czy ejst ACCEPTED DENIED, w zaleznosci od tego co będzie innaczej rozwinie sie akcja
				//2222222222222222222222222222222222
				welcomeUserText = userText.getText();
				userNameSendString = welcomeUserText;
				passw = passwordText.getText();
			
				
				toSendString = userNameSendString+ "\n" + passw +"\nLOG_IN\n\n";
				
				toSendChar = toSendString.toCharArray();
				sent = new byte[4096];
				
				for(int i=0 ; i<toSendChar.length ; i++)
					sent[i] = (byte) toSendChar[i];
				
				writerPipe.write(sent);
				received = readerPipe.read();
				response = new char[4096];
				
				
				for(int i=0 ; i<received.length ; i++)
					response[i] = (char) received[i];
				
				
				responseAnswer = response.toString();
				responseAnswerSplit = responseAnswer.split("\n");
				part1 = responseAnswerSplit[0];
				part2 = responseAnswerSplit[1];
				if(part1.equals("ACCEPTED"))
				{
					frame.getContentPane().removeAll();
					secondPanel();
					
					frame.repaint();
					frame.setVisible(true);
				}
				else
				{
					JOptionPane.showMessageDialog(null,part2,"Błąd logowania",JOptionPane.INFORMATION_MESSAGE);
				}
						
					
			}
			
			else if (e.getSource() == guestButton ) 
			{
				
				welcomeUserText = "Gość";
				userNameSendString = "Guest";
				passw = "123";
				frame.getContentPane().removeAll();
				secondPanel();
								
				frame.repaint();
				frame.setVisible(true);
				
			}
			else if (e.getSource()== downloadButton)
			{
			 //tutaj wrzucamy stuff do przekazania fifo	 (fifo read)
				
				
				
				passw = passwordText.getText();


		
				
				toSendString = userNameSendString+ "\n" + passw +"\nDOWNLOAD\n/\n"+fileNameToGive;
				
				toSendChar = toSendString.toCharArray();
				sent = new byte[4096];
				
				for(int i=0 ; i<toSendChar.length ; i++)
					sent[i] = (byte) toSendChar[i];
				
				
				
				writerPipe.write(sent);

				responseAnswer = response.toString();
				responseAnswerSplit = responseAnswer.split("\n");
				part1 = responseAnswerSplit[0];
				part2 = responseAnswerSplit[1];
				if( part1.equals("ACCEPTED")) 
				{
					bytesIntCommand = ByteBuffer.allocate(4).putInt(300).array();
					
					writerPipe.write(bytesIntCommand);
				}
				else
				{
					bytesIntCommand = ByteBuffer.allocate(4).putInt(100).array(); 

					writerPipe.write(bytesIntCommand);
					JOptionPane.showMessageDialog(null,part2,"Błąd pobierania",JOptionPane.INFORMATION_MESSAGE);
				}
					
				
			
			}
			else if (e.getSource()== uploadButton)
			{
				//tutaj stuff do przekazania fifo (fifo write)
				
				passw = passwordText.getText();
				
				
				toSendString = userNameSendString+ "\n" + passw +"\nDOWNLOAD\n/\n"+uploadTextField.getText();
				
				toSendChar = toSendString.toCharArray();
				sent = new byte[4096];
				
				for(int i=0 ; i<toSendChar.length ; i++)
					sent[i] = (byte) toSendChar[i];
				
				writerPipe.write(sent);
				
				responseAnswer = response.toString();
				responseAnswerSplit = responseAnswer.split("\n");
				part1 = responseAnswerSplit[0];
				part2 = responseAnswerSplit[1];
				
				
				
				if(part1.equals("ACCEPTED"))
				{
					bytesIntCommand = ByteBuffer.allocate(4).putInt(200).array();
					
					writerPipe.write(bytesIntCommand);
				}
				else
				{
					bytesIntCommand = ByteBuffer.allocate(4).putInt(100).array();
					
					writerPipe.write(bytesIntCommand);
					JOptionPane.showMessageDialog(null,part2,"Błąd dodania",JOptionPane.INFORMATION_MESSAGE);
				}
				
			}
			else if (e.getSource()== closeButton)
			{
				p.destroy(); 
				System.exit(0);
				  
			}
			
			
			
			
			
		}
	
	
	}



	



	

	
