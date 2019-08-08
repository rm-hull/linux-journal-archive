import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class UedView extends JPanel implements Observer {

    Vector users = null;

	/** Construct a UedView */
    public UedView(Vector vu) {

		users = vu;
		setBorder(BorderFactory.createTitledBorder("User Editor View (read-only)"));

		UedTableModel dtm = new UedTableModel(users);
		JTable table = new JTable(dtm);
        table.setCellEditor(new DefaultCellEditor(new JTextField())); 

        // Construct a scroll pane and add the JTable into it. 
		JScrollPane scrollPane = new JScrollPane(table);

		// Add the scroll pane to this panel.
		setLayout(new GridLayout(1, 0)); 
        add(scrollPane);

    }

	public Dimension getPreferredSize() {
		return new Dimension(600, 300);
	}

	/** update is called by the Model when its data changes */
	public void update(Observable o, Object arg) {
	}
}
