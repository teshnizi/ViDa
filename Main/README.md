<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/white.png" width= "25%"><img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/ViDa.png" title="Logo" alt="Logo" width = "50%" align="center">

# VIDA

> A platform for converting SQL queries to C++ code and doing calculations on virtual data.

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

***A part of a sample code, generated with VIDA:***


<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/sample.png" title="sample code" alt="sample code">

---

## What is VIDA?

VIDA is a platform for converting sql queries into efficient c++ code. Beside conversion, it can use hostograms and distributions to do computations on data. This could be really helpful for large queries.
For instance, assume that you are going to find sum of all values of an integer column in a table. 
In a classic scenario, you have to iterate over all values and sum them up. In ViDa, you can assume the data is uniformly or normally distributed. You can even give a histogram as distribution input.
We call this data *virtual*. Now for calculating summation of all entries, you simply have to multiply the table size in mean of the distribution. 
Working with virtual data could be generalized to a large domain of computations: Grouping, Conditional Selection, ... 

-This platform works on TPCH database right now:
<a><img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/Documents/database_schema.png" title="sample code" alt="sample code" align="middle"></a>
---


## How Does it Work?

 The approach is similar to [Neumann, 2011](https://www.vldb.org/pvldb/vol4/p539-neumann.pdf). We represent each query with a tree, where each node stands for an operation. For instance, a sample tree for this query:
  
   `select *
    from R1,R3,
   (select R2.z,count(*)
    from R2
    where R2.y=3
    group by R2.z) R2
    where R1.x=7 and R1.a=R3.b and R2.z=R3.c`

could be this (Example is from the article):

<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/tree.png" width= "40%">

For more details, you could refer to [operators header file](https://gitlab.com/teshnizi/ViDaG/blob/master/Main/operators.h). Here is a list of implemented nodes:

*   **Node**: Generalized Node class. All other node classes inherit from this class.
*   **ScanNode**: Used for scanning a table.
*   **HashScanNode**: Used for scanning two tables jointly, based on a common value. For instance, assume that you want to find the total cargo size. 
        For each product, you need to find entries related to it in both part table and lineitem table. Then you should calculate *peartkey\_size \* lineitem\_quantity* and sum up all such values. 
        Here you can keep a hash table so you know which entry of the lineitem table is related to which entry of the part table, without itereating over the whole table.
*   **SelectNode**: Used for conditioning on values.
*   **AggregateNode**: Used for calculating functions of multiple entries (an example could be summation of all quantities).
*   **JoinNode**: Used for generating a new table by joining two different tables.
*   **GroupNode**: Used for grouping values based on a specific condition.
*   **WolframAggregateNode**: Used for sending distribution-related calculations to Wolfram API, receiveving equivalent expressions and incorporating them into the generated code. Wolfram Alpha web API is not strong enough for calculating complex expressions,
    so we have used Mathematica instead. Mathematica is available for free on Raspberry Pi boards, and this node sends a bash command to a Raspberry Pi board via ssh:\
    `string ret = "ssh pi@192.168.43.187 \"wolframscript -format CForm -c '";`
    Therefore, in order to use distributions as data inputs, a Raspberry Pi board should be connected to your PC and ssh keys for connection should be already set up. You can take a look at [transmitter.h](https://gitlab.com/teshnizi/ViDaG/blob/master/Main/transmitter.h) for more details.
## Example
- We evaluate VIDA on two tpch queries: query 6 and query 19. You can find SQL codes [here](https://examples.citusdata.com/tpch_queries.html). The generated trees are as follows:
---

## License

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

- **[MIT license](http://opensource.org/licenses/mit-license.php)**
- Copyright 2015 © <a href="http://fvcproductions.com" target="_blank">FVCproductions</a>.