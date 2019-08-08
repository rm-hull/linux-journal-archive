package com.mcc.dataView;

import org.eclipse.ui.plugin.*;
import org.osgi.framework.BundleContext;
import java.util.*;

//MCC specific imports
import com.mcc.dataView.views.*;


/**
 * The main plugin class to be used in the desktop.
 */
public class DataViewPlugin extends AbstractUIPlugin {
	//The shared instance.
	private static DataViewPlugin plugin;
	//Resource bundle.
	private ResourceBundle resourceBundle;
	//User Parameter functionality
    public ParameterControl userParameters[] = new ParameterControl[100];
	
	/**
	 * The constructor.
	 */
	public DataViewPlugin() {
		super();
		plugin = this;
		try {
			resourceBundle = ResourceBundle.getBundle("com.mcc.dataView.DataViewPluginResources");
		} catch (MissingResourceException x) {
			resourceBundle = null;
		}
        // MCC Additions for User Parameter functionality
        int index;
        for (index = 0; index < 100; index++)
        {
            userParameters[index] = new ParameterControl("Parameter "
                    + (index + 1), "Value " + (index + 1));
        }
	}

	/**
	 * This method is called upon plug-in activation
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
	}

	/**
	 * This method is called when the plug-in is stopped
	 */
	public void stop(BundleContext context) throws Exception {
		super.stop(context);
	}

	/**
	 * Returns the shared instance.
	 */
	public static DataViewPlugin getDefault() {
		return plugin;
	}

	/**
	 * Returns the string from the plugin's resource bundle,
	 * or 'key' if not found.
	 */
	public static String getResourceString(String key) {
		ResourceBundle bundle = DataViewPlugin.getDefault().getResourceBundle();
		try {
			return (bundle != null) ? bundle.getString(key) : key;
		} catch (MissingResourceException e) {
			return key;
		}
	}

	/**
	 * Returns the plugin's resource bundle,
	 */
	public ResourceBundle getResourceBundle() {
		return resourceBundle;
	}
}
