import java.awt.*;
import java.awt.event.*;

public class ExampleAWT extends Frame {
  ExampleAWT() {
    super("AWT");

    Label msgLabel = new Label("Quit?");
    Button yesButton = new Button("Yes");
    Button noButton = new Button("No");

    Panel buttonbox = new Panel();
    buttonbox.setLayout(new FlowLayout());
    buttonbox.add(yesButton);
    buttonbox.add(noButton);

    Panel msgbox = new Panel();
    msgbox.setLayout(new FlowLayout());
    msgbox.add(msgLabel);

    add(msgbox, BorderLayout.NORTH);
    add(buttonbox, BorderLayout.SOUTH);
    
    yesButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) { System.exit(0); }
    });

    noButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) { System.exit(1); }
    });

    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        System.exit(0);
      }
    });
  }

  public static void main(String[] args) {
    ExampleAWT frame = new ExampleAWT();
    frame.pack();
    frame.setVisible(true);
  }
}
