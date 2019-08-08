package com.mcc.dataView.views;

/**
 * A multiple parameter class for use in a TableViewer
 * A ParameterControl has a Name and a Value
  */
public class ParameterControl {
	private String parameterName 	= "";
	private String parameterValue 	= "";

	/**
	 * Create parameter with initial Name and Value
	 * @param string
	 */
	public ParameterControl(String name, String value) {		
		super();
		setParameterName(name);
		setParameterValue(value);
	}

	/**
	 * Get parameterName
	 * @return String parameterName
	 */
	public String getParameterName() {
		return parameterName;
	}

	/**
	 * Get parameterValue
	 * @return String parameterValue
	 */
	public String getParameterValue() {
		return parameterValue;
	}

	/**
	 * Set parameterName 
	 * @param string
	 */
	public void setParameterName(String string) {
		parameterName = string;
	}

	/**
	 * Set parameterValue
	 * @param string
	 */
	public void setParameterValue(String string) {
		parameterValue = string;
	}

}
