import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.layout.*;

public class ExampleSWT {
  public static void main(String[] args) {
    Display display = new Display();
    Shell shell = new Shell(display);
    shell.setLayout(new FillLayout(SWT.VERTICAL));

    Composite msgbox = new Composite(shell, SWT.NO_TRIM);
    RowLayout msglayout = new RowLayout();
    msglayout.justify = true;
    msgbox.setLayout(msglayout);

    Label label = new Label(msgbox, SWT.NO_TRIM);
    label.setText("Quit?");

    Composite buttonbox = new Composite(shell, SWT.NO_TRIM);
    RowLayout buttonlayout = new RowLayout();
    buttonlayout.justify = true;
    buttonlayout.pack = true;
    buttonbox.setLayout(buttonlayout);

    Button yesButton = new Button(buttonbox, SWT.PUSH); 
    yesButton.setText("Yes");
    Button noButton = new Button(buttonbox, SWT.PUSH); 
    noButton.setText("No");

    yesButton.addSelectionListener(new SelectionAdapter() {
      public void widgetSelected(SelectionEvent event) {
        System.exit(0);
      }
    });

    noButton.addSelectionListener(new SelectionAdapter() {
      public void widgetSelected(SelectionEvent event) {
        System.exit(1);
      }
    });
    
    shell.pack();
    shell.open();

    while (! shell.isDisposed()) {
      if (! display.readAndDispatch()) display.sleep();
    }
  }
}
