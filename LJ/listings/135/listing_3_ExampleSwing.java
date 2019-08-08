import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;

public class ExampleSwing {
  public static void main(String[] args) {
    JFrame win = new JFrame("Swing");

    JLabel msgLabel = new JLabel("Quit?");
    JButton yesButton = new JButton("Yes");
    JButton noButton = new JButton("No");
    
    win.getContentPane().setLayout(new BorderLayout());

    JPanel buttonbox = new JPanel();
    buttonbox.setLayout(new FlowLayout());
    buttonbox.add(yesButton);
    buttonbox.add(noButton);
    
    win.getContentPane().add(msgLabel, "Center");        
    win.getContentPane().add(buttonbox, "South");  
    
    yesButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) { System.exit(0); }
    });

    noButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) { System.exit(1); }
    });

    win.pack();
    win.show();
  }
}
