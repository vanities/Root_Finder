/*	Adam Mischke
	Source.cpp
	Mon 14 Nov 2016 07:59:51 AM CST -> DUE Tue 15 Nov 2016 11:59:59 PM CST
	CSCI/MATH 3180 - Numerical Analysis
	
	Input:  A txt file
	Output: prints to std::out 

	Lab Assignment #5: Finding Roots using Newton's, Secant, and Bisection methods:

	This program dynammically finds the roots of polynomials using Newton's, Secant and Bisection methods
	using a txt file assigned below. The first line of the file represents the degree of the polynomial,
	the second line represents the coeffecients of the polynomial, and the third represents the domain of the
	x values being used to find the root. With the use of vectors, the data gets input and manipulated using the
	functions below. 
	
*/

#include <vector>		// vector and vector iterator
#include <iostream>		// std::cin, cout
#include <iomanip>		// std::fixed, std::setprecision(), std::setw()
#include <fstream>		// std::ifstream
#include <string>		// std::string
#include <math.h>		// std::fabs, std::pow
#include <time.h>		// std::clock, std::clock_t

// used these a lot
using std::vector;
using std::cout;

// change this to change the max iterations 
const int MAX_ITER = 15;

// Gets the input in!
// Input:  The input file stream, the vector of vectors of coeffecients, the domain values
// Output: NONE
void GetIn(std::ifstream&, vector<vector<double>>&, vector<vector<double>>&);

// Prints the polynomial
// Input:  The iterator of the each polynomial
// Output: Prints to stdout
void PrintEquations(vector<vector<double>>::iterator);

// Prints the derivative of the polynomial
// Input:  The iterator of the each polynomial
// Output: Prints to stdout
void PrintDerivative(vector<vector<double>>::iterator);

// Performs Horners algorithm for the appropriate polynomial
// Input:  The iterator of the each polynomial, the domain, a flag for if it needs to calculate the poly or the derivative,
//		   an int for which domain to use (x0,x1, or c), and an int for which polynomial to us
// Output: Returns a 64-bit floating point answer
double Horners(vector<vector<double>>::iterator, vector<vector<double>>, bool, int, int);

// Performs the Newton–Raphson method for Zero finding algorithm
// Input:  The iterator of the each polynomial, the domain, and the counter for which polynomial to choose
// Output: Prints to stdout
void Newtons( vector<vector<double>>::iterator, vector<vector<double>>, int);

// Performs the Secant method for Zero finding algorithm
// Input:  The iterator of the each polynomial, the domain, and the counter for which polynomial to choose
// Output: Prints to stdout
void Secant(vector<vector<double>>::iterator, vector<vector<double>>, int);

// Performs the Bisection method for Zero finding algorithm
// Input:  The iterator of the each polynomial, the domain, and the counter for which polynomial to choose
// Output: Prints to stdout
void Bisection(vector<vector<double>>::iterator, vector<vector<double>>, int&);


int main() {

	//	declare the coeffficient, and x[0,1] vectors
	vector<vector<double>> coeff, x;

	// declare the filename
	std::string filename;

	// declare the counter for knowing which polynomial to use
	int counter = 0;

	cout << "Enter the input file name: ";
	std::cin >> filename;

	// for debugging quickly
	//filename = "data1.txt";
		
		// opens the input file from the filename
		std::ifstream in;
		in.open(filename);

		// set the fixed and precision of our 64-bit doubles
		cout << std::fixed << std::setprecision(9);
		
		// recieves all of the information and puts them into vectors 
		GetIn(in, coeff, x);

		// iterate through the rows
		for (vector<vector<double>>::iterator row = coeff.begin(); row != coeff.end(); ++row)
		{

			/*  Print the equations and derivatives being used */

			PrintEquations(row);
			PrintDerivative(row);
			
			/*	Perform Root Finding:
				1. Newtons   Method
				2. Secant    Method
				3. Bisection Method    */

			Newtons(row, x, counter);
			Secant(row, x, counter);
			Bisection(row, x, counter);

		}

		// Dr. Untch always said: " When ya open a file, ya gotta close it!! "
		in.close();
}

// Gets the input from input filestream and put into their 2d vectors: coefficients and domain
void GetIn(std::ifstream& input, vector<vector<double> >& coeff, vector<vector<double> >& x) {

	//  declare an iterator and the degree value of the polynomial
	int i = 0, degree;
	// declare a coeefficient in 64-bit floating point
	double coeffient;
	
	while (input.good()){

		// grab the degree
		input >> degree;
		
		// allot a new space for a vector
		coeff.push_back(vector<double>());
		x.push_back(vector<double>{0, 0, 0});

		// for loop for grabbing the coeffecients
		for (int j = 0; j < degree+1; j++) {

			input >> coeffient;
			coeff[i].push_back(coeffient);
		}

		// grabs the domain (x0 and x1)
		input >> x[i][0] >> x[i][1];

		// increment i
		i++;
	}	
}

// Prints the polynomials being evaluated
void PrintEquations(vector<vector<double>>::iterator row) {

	cout <<  "\n\nf(x) = ";
		
	// iterate through the coeffecients
	for (vector<double>::iterator coeff = row->end()-1; coeff != row->begin(); --coeff){

		// if the coeffecient is 0, don't bother displaying it
		if (*coeff != 0) 
			cout << *coeff << "x^" << coeff - row->begin() <<" + ";	
	}
	// prints off the last coeffecient ( x^0 )
	cout << *(row->begin()) << "\n";
}

// Prints the derivatives of the polynomials being evaluated
void PrintDerivative(vector<vector<double>>::iterator row) {

	cout << std::fixed << "f'(x) = ";

	// iterate through the coefficients
	for (vector<double>::iterator coeff = row->end() - 1; coeff != row->begin(); --coeff) {

		// print the last value (the one without an x now)
		if ((((coeff - row->begin()) - 1) == 0))
			cout << ((coeff - row->begin()) * *coeff);

		//  else if the coeffecient is 0, don't bother displaying it
		else if (*coeff != 0)
			cout << ((coeff - row->begin()) * *coeff)  << "x^" << (coeff - row->begin())-1 << " + ";
		
	}
	cout << "\n";
}

// Performs Horners algorithm and evaluates the polynomial or derivative
double Horners(vector<vector<double>>::iterator row, vector<vector<double>> x, bool flag, int xcount, int count) {

	// initialize an initial x
	double init = x[count][xcount], xeval = 0;

	// If flag is 1, evaluate the polynomial
	if (flag == 1) {

		// iterate through the columns
		for (vector<double>::iterator col = row->end()-1; col != row->begin(); --col) {
			
			// add all the coeffecients that aren't 0
			if (*col != 0)
				xeval += *col * pow(init, (col - row->begin()));

		}
		// add the last coeffecient (x0)
		xeval += *(row->begin());

		// return the evaluated polynomial
		return xeval;
	}
	// Else if the flag is 0, evaluate the derivative
	/*	ONLY USED IN NEWTONS Method   */
	else {
		// iterate through the columns
		for (vector<double>::iterator col = row->end() - 1; col != row->begin(); --col) {

			// add the last coeffecient (x0)
			if ((((col - row->begin()) - 1) == 0))
				xeval += ((col - row->begin()) * *col);
				
			// add all the coefficients that aren't 0
			else if (*col != 0) 
				xeval += ((col - row->begin()) * *col) * pow(init, (col - row->begin()) - 1);
		
		}
		// return the evaluated derivative of the polynomial
		return xeval;
	}
}

// Performs the Newton–Raphson method for Zero finding algorithm
void Newtons(vector<vector<double>>::iterator row, vector<vector<double>> x, int count) {

	cout << "-----------------------------------------------------------\nNewton's Method\n-----------------------------------------------------------\n";

	cout << "Iteration(i)" << std::setw(11) << "xi" << std::setw(16) << "f(xi)" << std::setw(16) << "f'(x)\n";

	std::clock_t t1, t2;
	t1 = std::clock();

	for (int i = 0; i <= MAX_ITER; i++) {
		
		// if f(xn) < .000001 accuracy
		if (std::fabs(Horners(row, x, 1, 0, count)) < .000001)
			break;

		cout << std::setw(6) << i << std::setw(20) << x[count][0] << std::setw(15) << Horners(row, x, 1, 0, count) << std::setw(15) << Horners(row, x, 0, 0, count) << "\n";

		// perform newtons
		x[count][0] = x[count][0] - (Horners(row, x, 1, 0, count) / Horners(row, x, 0, 0, count));
	}

	t2 = std::clock();
	float diff ((((float)t2-(float)t1)) / CLOCKS_PER_SEC);
	cout << "Time taken to complete (in seconds): " << diff << "\n\n";
}

// Performs the Secant method for Zero finding algorithm
void Secant(vector<vector<double>>::iterator row, vector<vector<double>> x, int count) {

	cout << "Secant Method\n--------------------------------------------------------------------------\n";

	cout << "Iteration(i)" << std::setw(11) << "xi" << std::setw(16) << "xi+1" << std::setw(15) << "f(xi)" << std::setw(18) << "f(xi+1)\n";
	double swap;

	std::clock_t t1, t2;
	t1 = std::clock();

	for (int i = 0; i <= MAX_ITER; i++) {

		// if f(xn) < .000001 accuracy
		if (std::fabs(Horners(row, x, 1, 1, count)) < .000001)
			break;

		cout << std::setw(6) << i << std::setw(20) << x[count][0] << std::setw(15) << x[count][1] << std::setw(15) << Horners(row, x, 1, 0, count) << std::setw(16) << Horners(row, x, 1, 1, count) << "\n";
		
		// put x+1 into a swap
		swap = x[count][1];
		// evaluate a new x+1
		x[count][1] = x[count][0] - (Horners(row, x, 1, 0, count) * ((x[count][1] - x[count][0]) / (Horners(row, x, 1, 1, count) - Horners(row, x, 1, 0, count))));
		// put x+1 into the old x
		x[count][0] = swap;
	}

	t2 = std::clock();
	float diff ((((float)t2-(float)t1)) / CLOCKS_PER_SEC);
	cout << "Time taken to complete (in seconds): " << diff << "\n";

	cout << "\n";
}

// Performs the Bisection method for Zero finding algorithm
void Bisection(vector<vector<double>>::iterator row, vector<vector<double>> x, int& count) {

	cout << "Bisection Method\n-------------------------------------------------------------------------\n";

	cout << "Iteration(i)" << std::setw(10) << "a" << std::setw(15) << "b" << std::setw(16) << "f(a)" << std::setw(16) << "f(b)\n";
	int i;

	// If both x0 and x1 are greater than 0 or less than 0, pick a new domain
	while ((Horners(row, x, 1, 0, count) > 0 && Horners(row, x, 1, 1, count) > 0) || (Horners(row, x, 1, 0, count) < 0 && Horners(row, x, 1, 1, count) < 0)) {
		cout << "\nError: Evaluation of x0 and x1 must have opposite signs.\n";
		cout << "Please select two more intervals for the domain of x, [x0,x1] -> enter like x0 x1: ";
		std::cin >> x[count][0] >> x[count][1];
		cout << "\n";
	}

	std::clock_t t1, t2;
	t1 = std::clock();


	for (i = 0; i <= MAX_ITER; i++) {

		// if f(xn) < .000001 accuracy
		if (std::fabs(Horners(row, x, 1, 2, count)) < .000001)
			break;

		cout << std::setw(6) << i << std::setw(20) << x[count][0] << std::setw(15) << x[count][1] << std::setw(15) << Horners(row, x, 1, 0, count) << std::setw(15) << Horners(row, x, 1, 1, count) << "\n";

		// take the median of two points
		x[count][2] = (x[count][0] + x[count][1]) / 2;

		// find and replace the x value: if f(c) is positive and f(a) is positive or if f(c) is negative and f(a) is negative
		if ((Horners(row, x, 1, 2, count) > 0 && Horners(row, x, 1, 1, count) > 0) || (Horners(row, x, 1, 2, count) < 0 && Horners(row, x, 1, 1, count) < 0))
			x[count][1] = x[count][2];
		else
			x[count][0] = x[count][2];

		cout << x[count][2] << Horners(row, x, 1, 2, count);

	}

	t2 = std::clock();
	float diff ((((float)t2-(float)t1)) / CLOCKS_PER_SEC);
	cout << "Time taken to complete (in seconds): " << diff << "\n";
	

	// if we left the loop condition and didn't find the root up to error < .000001, pick a new domain to look in
	if (i == MAX_ITER+1) {
		cout << "Please select two more intervals for the domain of x, [x0,x1] -> enter like x0 x1: ";
		std::cin >> x[count][0] >> x[count][1];
		Bisection(row, x, count);
		return;
	}
	
	// We're done! Increase the counter so we can move to the next polynomial
	count++;
}