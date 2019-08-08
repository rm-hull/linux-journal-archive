package com.mcc.dataView.views;


import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.*;
import org.eclipse.jface.viewers.*;
//import org.eclipse.swt.graphics.Image;
import org.eclipse.jface.action.*;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.*;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.SWT;

// MCC Additional Imports
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.widgets.*;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.swt.layout.*;

//MCC specific imports
import com.mcc.dataView.DataViewPlugin;


/**
 * This sample class demonstrates how to plug-in a new
 * workbench view. The view shows data obtained from the
 * model. The sample creates a dummy model on the fly,
 * but a real implementation would connect to the model
 * available either in this or another plug-in (e.g. the workspace).
 * The view is connected to the model using a content provider.
 * <p>
 * The view uses a label provider to define how model
 * objects should be presented in the view. Each
 * view can present the same model objects using
 * different labels and icons, if needed. Alternatively,
 * a single label provider can be shared between views
 * in order to ensure that objects of the same type are
 * presented in the same way everywhere.
 * <p>
 */

public class DataView extends ViewPart {
	private TableViewer viewer;
	private Action action1;
	private Action action2;
	private Action doubleClickAction;
	private DataViewPlugin dview;

	/*
	 * The content provider class is responsible for
	 * providing objects to the view. It can wrap
	 * existing objects in adapters or simply return
	 * objects as-is. These objects may be sensitive
	 * to the current input of the view, or ignore
	 * it and always show the same content 
	 * (like Task List, for example).
	 */
	 
	class ViewContentProvider implements IStructuredContentProvider
	{
		private int howMany = 0;
		public void inputChanged(Viewer v, Object oldInput, Object newInput)
		{
		}
		public void dispose()
		{
		}
		public ViewContentProvider(int count)
		{
			howMany = count;
		}
		public Object[] getElements(Object parent)
		{
			return dview.userParameters;
		}
	}
	
	//class NameSorter extends ViewerSorter {
	//}

	/**
	 * The constructor.
	 */
	public DataView() {
	}

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent)
	{
		Font font;
		int index = 0;
		int count = 100;
		Table table;

		dview = DataViewPlugin.getDefault();
		font = new Font(parent.getDisplay(),
				 new FontData("Courier New", 10, SWT.NORMAL));

		int style =
			SWT.SINGLE
				| SWT.BORDER
				| SWT.H_SCROLL
				| SWT.V_SCROLL
				| SWT.FULL_SELECTION
				| SWT.HIDE_SELECTION
				| SWT.CENTER
				| SWT.VERTICAL;
		table = new Table(parent, style);

		viewer = new TableViewer(table);
		table.setHeaderVisible(true);
		viewer.setUseHashlookup(true);
		viewer.setContentProvider(new ViewContentProvider(count));
		viewer.setLabelProvider(new ViewLabelProvider());
		viewer.setInput(ResourcesPlugin.getWorkspace());
		table.setLinesVisible(true);
		GridData gridData =
			new GridData(GridData.HORIZONTAL_ALIGN_FILL | GridData.FILL_BOTH);
		table.setLayoutData(gridData);
		GridLayout layout = new GridLayout(2, false);
		layout.marginWidth = 4;
		table.setLayout(layout);
		table.select(0);
		table.setFont(font);
		
		// 1st column with user parameter names
		TableColumn column = new TableColumn(table, SWT.LEFT, 0);
		column.setWidth(250);
		column.setText("Parameter Name");

		// 2nd column with user parameter values
		column = new TableColumn(table, SWT.CENTER, 1);
		column.setWidth(250);
		column.setText("Parameter Value");
		
		makeActions();
		hookContextMenu();
		hookDoubleClickAction();
		contributeToActionBars();
		updateTheTable();
	}


	private void hookContextMenu() {
		MenuManager menuMgr = new MenuManager("#PopupMenu");
		menuMgr.setRemoveAllWhenShown(true);
		menuMgr.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager manager) {
				DataView.this.fillContextMenu(manager);
			}
		});
		Menu menu = menuMgr.createContextMenu(viewer.getControl());
		viewer.getControl().setMenu(menu);
		getSite().registerContextMenu(menuMgr, viewer);
	}

	private void contributeToActionBars() {
		IActionBars bars = getViewSite().getActionBars();
		fillLocalPullDown(bars.getMenuManager());
		fillLocalToolBar(bars.getToolBarManager());
	}

	private void fillLocalPullDown(IMenuManager manager) {
		manager.add(action1);
		manager.add(new Separator());
		manager.add(action2);
	}

	private void fillContextMenu(IMenuManager manager) {
		manager.add(action1);
		manager.add(action2);
		// Other plug-ins can contribute their actions here
		manager.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS));
	}
	
	private void fillLocalToolBar(IToolBarManager manager) {
		manager.add(action1);
		manager.add(action2);
	}

	private void updateTheTable()
	{
		String output = "1234";
		int count = 100;
		dview.userParameters[0] = new ParameterControl ("Parameter " + 1, output);
		dview.userParameters[1] = new ParameterControl ("Parameter " + 2, "FEED");
		dview.userParameters[2] = new ParameterControl ("Parameter " + 3, "FACE");
		dview.userParameters[3] = new ParameterControl ("Parameter " + 4, "5678");
		viewer.setContentProvider(new ViewContentProvider(count));
	}


	private void makeActions() {
		action1 = new Action() {
			public void run() {
				showMessage("Action 1 executed");
			}
		};
		action1.setText("Action 1");
		action1.setToolTipText("Action 1 tooltip");
		action1.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().
			getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));
		
		action2 = new Action() {
			public void run() {
				showMessage("Action 2 executed");
			}
		};
		action2.setText("Action 2");
		action2.setToolTipText("Action 2 tooltip");
		action2.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().
				getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));
		doubleClickAction = new Action() {
			public void run() {
				//ISelection selection = viewer.getSelection();
				//Object obj = ((IStructuredSelection)selection).getFirstElement();
				//showMessage("Double-click detected on "+obj.toString());
				showMessage("Double-click detected on "
						+dview.userParameters[viewer.getTable().getSelectionIndex()].getParameterName()
						+ " with Value of "
						+ dview.userParameters[viewer.getTable().getSelectionIndex()].getParameterValue());
			}
		};
	}

	private void hookDoubleClickAction() {
		viewer.addDoubleClickListener(new IDoubleClickListener() {
			public void doubleClick(DoubleClickEvent event) {
				doubleClickAction.run();
			}
		});
	}
	private void showMessage(String message) {
		MessageDialog.openInformation(
			viewer.getControl().getShell(),
			"Data View",
			message);
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		viewer.getControl().setFocus();
	}
}