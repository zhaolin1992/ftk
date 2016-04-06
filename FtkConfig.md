# Introduction #

The config options in FTK


# Config options: #

  * statusbar enable=?
> > control if enable statusbar.

  * cursor enable=?
> > control if enale cursor.

  * theme name=?
> > set the name of theme.

  * data\_dir value=?
> > set the data directory.

  * test\_data\_dir value=?
> > set the test data directory.

# Load Order #

  * Initialize with default values.
  * Load FTK\_CNF
  * Load ftk.cnf in current working directory.

# Example: #



&lt;statusbar     enable="1" /&gt;




&lt;cursor        enable="0" /&gt;




&lt;theme         name="default" /&gt;




<data\_dir      value="./data" />




<test\_data\_dir value="./testdata" />




