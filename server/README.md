README.md

a. Name: Yueh-Hsun Lin

b. Student ID: 2926205064

c.

d. The code files are:
	aws.c: This is the AWS server code file and it will be receiving input and function from the client and sending these to backend servers for power calculations. It will later receive back from the backend servers. Finally, it will calculate the corresponding result and send it back to the client.

	client.c: This is the client code file. It will be connecting to the AWS server and sending in the function and input that was specified in the command line. Finally, it will receive the calculated results from the AWS server.

	serverA.c: This is the code file for the backend server for square calculation.

	serverB.c: This is the code file for the backend server for cube calculation.

	serverC.c: This is the code file for the backend server for fifth power calculation.

e.
	+=========+=========+=======================+========+
	|  From   |   To    |        Message        | Format |
	+=========+=========+=======================+========+
	| Client  | AWS     | Input                 | String |
	+---------+---------+-----------------------+--------+
	| AWS     | Client  | Ack                   | String |
	+---------+---------+-----------------------+--------+
	| Client  | AWS     | Function              | String |
	+---------+---------+-----------------------+--------+
	| AWS     | ServerA | Input                 | String |
	+---------+---------+-----------------------+--------+
	| AWS     | ServerB | Input                 | String |
	+---------+---------+-----------------------+--------+
	| AWS     | ServerC | Input                 | String |
	+---------+---------+-----------------------+--------+
	| ServerA | AWS     | Input squared         | String |
	+---------+---------+-----------------------+--------+
	| ServerB | AWS     | Input cubed           | String |
	+---------+---------+-----------------------+--------+
	| ServerC | AWS     | Input to fifth Power  | String |
	+---------+---------+-----------------------+--------+
	| AWS     | ServerA | Input squared         | String |
	+---------+---------+-----------------------+--------+
	| AWS     | ServerB | Input squared         | String |
	+---------+---------+-----------------------+--------+
	| ServerA | AWS     | Input to fourth power | String |
	+---------+---------+-----------------------+--------+
	| ServerB | AWS     | Input to sixth power  | String |
	+---------+---------+-----------------------+--------+
	| AWS     | Client  | Result                | String |
	+---------+---------+-----------------------+--------+

f.	My code may terminate and print error messages if user inputs are different from those specified in the project assignment. For example, if input is not "LOG" but "log" (case sensitive), or if the input value is out of the range of 0 to 1, there will be an error. It is mentioned by a TA that we can assume the input will be corrent and valid, so there should not be cases where my program terminates in error.

g.	I reused some code snippets from Beej's guide on setting up the structs and doing simple operations such as but not limited to binding and listening. I have an inline comment in the code above the code I used.
