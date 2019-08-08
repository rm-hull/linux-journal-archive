package com.mcc.dataView.views;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

/**
 * Label provider for a TableViewer
 */
public class ViewLabelProvider 
    extends LabelProvider
    implements ITableLabelProvider {

    public String getColumnText(Object elem, 
                                int index) {
        String result = "";
        ParameterControl param = 
            (ParameterControl) elem;
            switch (index) {
            case 0: 
                result = param.getParameterName();
                break;
            case 1 :
                result = param.getParameterValue();
                break;
            default :
                break; 	
            }
            return result;
    }

    public Image getColumnImage(Object element, 
                                int columnIndex) {
        return(null);
    }

}
