<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/white.png" width= "25%"><img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/ViDa.png" title="Logo" alt="Logo" width = "50%" align="center">

# VIDA

> A platform for converting SQL queries to C++ code and doing calculations on virtual data.

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

***A part of a sample code, generated with VIDA:***


<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/sample.png" title="sample code" alt="sample code">


## What is VIDA?

VIDA is a platform for converting sql queries into efficient c++ code. Beside conversion, it can use hostograms and distributions to do computations on data. This could be really helpful for large queries.
For instance, assume that you are going to find sum of all values of an integer column in a table. 
In a classic scenario, you have to iterate over all values and sum them up. In ViDa, you can assume the data is uniformly or normally distributed. You can even give a histogram as distribution input.
We call this data *virtual*. Now for calculating summation of all entries, you simply have to multiply the table size in mean of the distribution. 
Working with virtual data could be generalized to a large domain of computations: Grouping, Conditional Selection, ... 

The platform is implemented based on the TPCH database, but is easily generalizable to any database.

<a><img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/Documents/database_schema.png" title="sample code" alt="sample code" width="70%"></a>


## How Does it Work?

 The approach is similar to [Neumann, 2011](https://www.vldb.org/pvldb/vol4/p539-neumann.pdf). We represent each query with a tree, where each node stands for an operation. For instance, a sample tree for this query:
  
  
```sql
        select *
    from R1,R3,
   (select R2.z,count(*)
    from R2
    where R2.y=3
    group by R2.z) R2
    where R1.x=7 and R1.a=R3.b and R2.z=R3.c

```

  
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

The definition of `Node` is as follows:
```cpp
class Node{
public:
    Node(string name){
        this->name = name;
    }
    Node(string name, Node* parent) : Node(name){
        this->parent = parent;
        parent->child = this;
    }
    virtual void produce(set<Attribute> *a, set<Table> tables, set<string> *fixed_tables) {
        this->child->produce(a, tables, fixed_tables);
//        while(!indent.empty()) {
//            indent = indent.substr(0, indent.size()-1);
//            fprintf(pfile,"%s}\n", indent.c_str());
//        }
    };

    virtual void consume(set<Attribute> *a, set<Table> tables, Node *source, set<string> *fixed_tables) {
        return;
    }

    void prep(){}

    virtual string get_class(){
        return "Node";
    }

    void set_child(Node* child){
        this->child = child;
    }
    Node* child{};
    Node* parent{};
    string name;
};
```

In order to generate code for a specific sql query, first we define database column names as a set of `Attribute`s. Then we define needed nodes and make the tree by connecting nodes to each other. 
After tree definition, we print librarly inclusion codes, main header, and database loading codes. Then we call `.prep()` function for each node. This function generates 
the prerequisite codes (things like variable definition, creating hash table, allocating memory, ...). Finally we generate the main body of the code by calling `root.produce(&attributes, tables, &x);` where tables is set of all tables and x is an empty set of strings.
## Example
 We evaluate VIDA on two tpch queries: query 6 and query 19. You can find SQL codes [here](https://examples.citusdata.com/tpch_queries.html). Let's look how does the code work for query 6 first. The corresponding tree is:

<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/white.png" width = "40%">
<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/Q6_tree.png">

So we can define the following nodes:
```cpp
        OperandNode opd2(&l_discount_att);
        OperandNode opd3(&l_price_att);
        OperatorNode opr2(&opd3, &opd2, false, "mult", false);

        AggregateNode aggregateNode = AggregateNode("sum_price", &root, agg_sum, data_att, &opr2);
        SelectNode selectNode = SelectNode("Select", &aggregateNode, var, 1, ranges, strings, 0, valid_strings);
        ScanNode scanNode1 = ScanNode("ScanL", &selectNode);
        
        ...some preprocess codes here...
        
        

```

Then we call `prep()` function for each node, which generates code for defining variables and coing preprocess. Then we print code for loading tables into the file (which could be easily automated if databases are generalized). 
and call `root.produce(&attributes, tables, &x);`. You can read [the generator code](https://gitlab.com/teshnizi/ViDaG/blob/master/Main/Q6_operators.cpp) or [the generated code](https://gitlab.com/teshnizi/ViDaG/blob/master/Main/Sample%20Generated%20Codes/Q6.cpp) (comment generation is autonomous too) 
for more details. (We have assumed l_discount to be a histogram rather than real data in this sample)


In the other example, you can see how does VIDA use distributions. The tree for query 19 is something like this:

<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/white.png" width = "35%">
<img src="https://gitlab.com/teshnizi/ViDaG/raw/master/Main/Q19_tree.png">

As you can see, we are using a hash scan node rather than a scan node, as we are going to join two tables based on a common attribute. 
We compute some more complex expression instead of `sum(l_price* (1 - l_discount)` to show how useful distributions could be. Our computation target is `(l_price ^ 2 ) * ((1 + l_discount)) + psize`.
Let 
```math
psize \sim \mathcal{N}ormal(4,1)
```

```math
ldiscount \sim \mathcal{N}ormal(2,2)
```
## License

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

- **[MIT license](http://opensource.org/licenses/mit-license.php)**
- Copyright 2015 © <a href="http://fvcproductions.com" target="_blank">FVCproductions</a>.