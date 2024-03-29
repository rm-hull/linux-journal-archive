Hello,

I am enjoying the current issue of LJ and downloaded some code to follow
along with the author the article "Financial Calculation Programs for
Linux".  I did not check the Perl or C code, but since I have some
familiarity with Java, I did check it.  It had some misplaced quotation
marks which took the place of other necessary characters in a couple of
places.  I am attaching a corrected version of the code - it compiles
and runs just fine now.  I also placed braces on their own lines which
is how I normally write them,  just to  make it easier for me to
analyze.

Dave Bunker
(debunk@borg.com)

-------------------------------------------------------------------------

import java.io.*;
import java.util.*;
import java.text.*;

public class Irr implements Do_irr_returns
{
   public static void main(String[] argv)
   {
      int iters_max = 20;
      double epsilon = 1.0e-6;
      MyInt iterations = new MyInt();
      MyDouble du = new MyDouble(0.0);
      FileReader file_reader;
      BufferedReader buffered_reader;
      StreamTokenizer t;
      Expenses add_expense;
      Object[] ro_args = new Object[2];
      Vector positives = new Vector(), negatives = new Vector();

      if(argv.length != 1) {
         System.out.println("Usage: Irr <data file>\n");
         return;
      }
      try {
        file_reader = new FileReader(argv[0]);
      } catch(java.io.FileNotFoundException x) {
        System.out.println("Could not open \"" + argv[0] + "\"...");
        return;
      }
      buffered_reader = new BufferedReader(file_reader);
      String line;
      System.out.println(" Income($)     Time (yr)\n" +
                         " =========     =========");
      for(;;) {
        try {
          line = buffered_reader.readLine();
        } catch(IOException x) {
          System.out.println("Error reading file...");
          return;
        }
        if(line == null) {
          break;
        }
        StringTokenizer st = new StringTokenizer(line, ", ");
        add_expense = new Expenses();
        add_expense.amount = Double.valueOf(st.nextToken()).doubleValue();
        add_expense.time   = Double.valueOf(st.nextToken()).doubleValue();
        ro_args[0] = new Double(add_expense.amount);
        ro_args[1] = new Double(add_expense.time);
        System.out.println(MessageFormat.format(
                   "{0,number,#######.00}       {1,number,##.00}", ro_args));
        if(add_expense.amount > 0.0) {
          positives.addElement(add_expense); 
        } else if(add_expense.amount < 0.0) {"
          add_expense.amount = -add_expense.amount;"
          negatives.addElement(add_expense); 
        } else {
          System.out.println("Cannot have dollar amount = $0.00 !...");
          return;
        }
      }
      System.out.println();
      if(positives.size() == 0 || negatives.size() == 0) {
        System.out.println(
                  "\nCannot calculate irr. Need income AND expenditures!...");
        return;
      }
      switch(Do_irr(positives, negatives, iters_max, epsilon, du,
                    iterations)) {
      case OK:
         ro_args[0] = new Double(100.0 * (Math.exp("du.a_double) " 1.0));
         ro_args[1] = new Double(-du.a_double * 100.0);
         String s = MessageFormat.format(
            "{0,number,##.0000}% (discrete) = {1,number,##.0000}%", ro_args);
         System.out.println("\nIRR = " + s + " (continuous) after " +
                            iterations.an_int + " iteration" +
                            (iterations.an_int == 1 ? "" : "s"));
         break;
      default:
      case NO_CONVERGE:
         System.out.println(
            "irr() does not converge in " + iters_max + " iterations\n");
         break;
      }
   }

   static int Do_irr(Vector positives, Vector negatives, int iters_max,
                     double epsilon, MyDouble du, MyInt iterations)
  {
    int iters, i, rv;
    double u = du.a_double, pos, d_pos, neg, d_neg, delta, tmp, time;

    for(iters = iters_max; iters > 0; iters--) {
      for(i = 0, pos = d_pos = 0.0; i < positives.size(); i++) {
        time = ((Expenses)positives.elementAt(i)).time;
        tmp  = ((Expenses)positives.elementAt(i)).amount * Math.exp(u * time);
        pos += tmp;
        d_pos += tmp * time;
      }
      for(i = 0, neg = d_neg = 0.0; i < negatives.size(); i++) {
        time = ((Expenses)negatives.elementAt(i)).time;
        tmp  = ((Expenses)negatives.elementAt(i)).amount * Math.exp(u * time);
        neg += tmp;
        d_neg += tmp * time;
      }
      delta = Math.log(neg / pos) / (d_neg / neg - d_pos / pos);
      if(Math.abs(delta) < epsilon) {
      break;
      }
      u -= delta;
    }
    if(iters == 0) {
      rv = NO_CONVERGE;
    } else {
      rv = OK;
      du.a_double = u;
      iterations.an_int = iters_max - iters + 1;
    }
    return rv;
  }
}

class Expenses
{
  double amount;
  double time;
}

interface Do_irr_returns
{
   static final int OK          = 0;
   static final int NO_CONVERGE = 1;
}

class MyDouble
{
  double a_double;

  MyDouble(double d)
  {
    a_double = d;
  }
}

class MyInt
{
   int an_int;
}

