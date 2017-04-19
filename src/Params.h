/*
 * params.h
 *
 *  Created on: 19.04.2017
 *      Author: Christian
 */

#ifndef SRC_PARAMS_H_
#define SRC_PARAMS_H_

extern const char* P_INSTANCE;
extern int P_IP_TIMEOUT;
extern int P_IP_THREADS;

void printParams();
void readParams(int argc, char* argv[]);

#endif /* SRC_PARAMS_H_ */
