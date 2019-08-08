/*

Three-tier Hangman
Copyright (C) 2000 Ariel Ortiz mailto:aortiz@acm.org

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
02111-1307, USA.

*/

import java.awt.*;

/**
 * Stickman image drawing class.
 * @author: Ariel Ortiz
 */
class StickMan extends Canvas {
	private int opportunitiesLeft;
/**
 * Create new Stickman insatance.
 */
public StickMan() {
	setSize(120, 150);
	reset();
	}
/**
 * Overrides conventional (do-nothing) canvas paint method.
 * @param g graphics context.
 */
public void paint(Graphics g) {
	g.fillRect(20, 10, 60, 10);
	g.fillRect(20, 20, 10, 80);
	g.fillRect(10, 100, 90, 10);
	int[] x = {40, 50, 30, 30};
	int[] y = {20, 20, 40, 30};
	g.fillPolygon(x, y, 4);
	g.drawLine(70, 20, 70, 30);
	if (opportunitiesLeft > -1) {
		if (opportunitiesLeft < 6) {
			// Draw head
			g.drawOval(60, 30, 20, 20);
		}
		if (opportunitiesLeft < 5) {
			// Draw body line
			g.drawLine(70, 50, 70, 70);
		}
		if (opportunitiesLeft < 4) {
			// Draw left arm
			g.drawLine(70, 55, 50, 55);
		}
		if (opportunitiesLeft < 3) {
			// Draw right arm
			g.drawLine(70, 55, 90, 55);
		}
		if (opportunitiesLeft < 2) {
			// Draw left leg
			g.drawLine(70, 70, 50, 90);
		}
		if (opportunitiesLeft < 1) {
			// Draw right leg
			g.drawLine(70, 70, 90, 90);
		}
	}
}
/**
 * Resest a StickMan instance, so that there is no stickman when 
 * initially drawn.
 */
public void reset() {
	opportunitiesLeft = -1;
}
/**
 * Set the player's number of remaining opportunities.
 * @param n how many opportunities does the player have before being completely hung.
 */
public void setOpportunitiesLeft(int n) {
	opportunitiesLeft = n;
	repaint();
}
}
