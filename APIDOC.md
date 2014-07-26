# API

## API function /update

This is of type:

```
[
{
	"cmd" : "SET",
	"node" : { "id":"PERS_abcde" },
	"index" : { "mob":"9999900000" },
	"noindex" : { "age":12222,"name":"rama" }
},
{
	"cmd" : "SET",
	"node" : { "id":"PERS_pqrst" },
	"index" : { "mob":"9999911111" }
	"noindex" : { "name":"bobo" }
},
{
	"cmd" : "SET",
	"edge" : {
		"src" : { "id":"PERS_abcde" },
		"trg" : { "id":"PERS_pqrst" },
		"type" : "KNOWS"
	},
	"noindex" : { "age":12222}
}
]
```

This is an array of objects to set, each having

* "cmd" : "SET",
* edge OR node 
* index : indexable parameters ( node only )
* noindex : non-indexable parameters

### Field "cmd"

So far only SET is implemented

### Field "node"

This field if prersent is the item being inserted is a 
Must have a field `id` . 

### Field "edge"

Must have three fields `src`, `trg` and `type` . The fields `src` and `trg` are to contain id of source and target nodes.
A Field  `type` is string.

### Field "index"

Fields to be indexed

### Field "noindex"

Fields not to be indexed

-----------


## API function /query

This is of type:

```
{
"match":"A -- KNOWS -> B -- LIKED -> C",
"return":[ ..]
"where": [ .. ]
"orderby":["A.id ASC","b.id DESC"],
"limit":100,
"offset":100
}
```

### Field "match"
Exressed as a list of node and several predicates ( RELN + NODE) the items contained here are treated as primary keyword. 
Node/Relation names must begin with alphabets and can contain alphabets and underscore. The direction of relationship may be forward (->) or reverse (<-) or both (<>).

**Example**: For Nodes A, B,C and relations KNOWS, LIKED

	A -- KNOWS -> B -- LIKED -> C
	A -- KNOWS -> B -- LIKED -> C -- BELONGSTO <- D

### Field "return"

Items to return expressed as an array or a comma separated string of keyword.field. Example

	"return":"A.id,A.name"
		OR
	"return":["A.id","A.name"]
	

### Field "where"

The "where" clause comprises of logic blocks that in turn has filters.

**Filters**  are expressed as values of keyword.field

* single value like ```"A.id":"PERS_abcde"```.
* array of several possible values treated as OR like 
```"A.id":["PERS_abcde","PERS_abcdf"]```.
* range object using `gt` , `ge` , `lt`, `le` , `eq` , `neq` like

```
"A.age":{"ge":10,"le":20} // 10 <= A.age <= 20
"A.age":{"gt":10,"lt":20} // 10 < A.age < 20
"A.age":{"gt":10} // A.age > 10
{"A.id":{"neq":10}} // A.id not 10
{"A.id":{"neq":"test"}} // A.id string not "test"
```

**Logic Block** is made up of filter with logic , an array of these make up the "where" value. For example

```
[{
	"logic":"OR",
	"A.id":["PERS_abcde","PERS_abcdf"],
	"A.mob":"9999900000"
},
{
	"logic":"AND",
	"B.age":{"ge":18},
d	"B.has_pets":{"gt":0}
}]
```
**Note:** In a "where" clause there should be at least one top-level filter for the starting node. Compared with SQL only AND is supported at root level (implicit).

### Field "orderby"

Ordering is expressed as keyword.field order and a keyword `ASC` or `DESC`.
Arrays are used for multiple ordering clause. So the first array element is the primary sorting order,
within which the similar values are farther sorted by second element.
Format could be array of string or a comma separated single string list of items where each item
is of the form `keyword.field ASC` or `keyword.field DESC` . Examples:

```
"orderby":["A.id ASC","b.id DESC"]
	OR
"orderby":"A.id ASC , b.id DESC"
```

### Field "limit"
No. of results to retrieve like 

```"limit":100```

### Field "offset"
No. of results to skip (optional, default 0) like 

```"offset":100``` // starts from 101th 

### Similarity with SQL

```
SELECT
	A.id,A.name,A.mobile,
	KNOWS.type,
	B.id,B.name,B.mobile,
	LIKED.type,LIKED.stars,
	C.id,C.name,C.category
FROM
	A -- KNOWS -> B -- LIKED -> C
WHERE
	TRUE
	AND (A.id IN ("PERS_abcde","PERS_abcdf") 
	OR A.mob = "9999900000" )
	AND A.banned=0
	AND ( KNOWS.type IN ("KNOWS","TKNOWS") )
	AND ( B.age >= 18 OR B.age >= A.age)
	AND ( LIKED.type = "LIKED" )
	AND ( C.id = "LIST_PXXX-2222" )
	AND ( C.id != "LIST_PXXX-2223" )
ORDERBY
	A.id ASC, B.id DESC
OFFSET
	100
LIMIT
	100
;
```

translates somewhat to this

```
{
"match":"A -- KNOWS -> B -- LIKED -> C",
"return":[
	"A.id","A.name","A.mobile",
	"KNOWS.type",
	"B.id","B.name","B.mobile",
	"LIKED.type","LIKED.stars",
	"C.id","C.name","C.category"
],
"where": [
	{
		"logic":"OR",
		"A.id":["PERS_abcde","PERS_abcdf"],
		"A.mob":"9999900000"
	},
	{"logic":"AND","A.banned":0},
	{"logic":"AND","KNOWS.type":["KNOWS","TKNOWS"]},
	{
		"logic":"OR",
		"B.age":{"ge":18},
		"B.age":{"ge":"A.age"}
	},
	{"logic":"AND","LIKED.type":"LIKED"},
	{"logic":"AND","C.id":"LIST_PXXX-2222"},
	{"logic":"AND","C.id":{"neq":"LIST_PXXX-2223"}}
],
 "orderby":["A.id ASC","b.id DESC"],
 "limit":100,
 "offset":100
}
```
