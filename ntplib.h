/*
 * ntplib.h
 * 
 * Copyright 2017  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifndef ntplib
#define ntplib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void startNTP(){
	system("sudo service ntp restart");
}

void stopNTP() {
	system("sudo service ntp stop");
}

int syncNTP(double *delay, double *offset, double *jitter, int print) {
	//returns 0 if unsynced and 1 if synced
	FILE *fp;
	char path[1035];
	char *syncAddr = "*192.168.42.1";
	
	fp = popen("ntpq -pn", "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(1);
	}
	
	while (fgets(path, sizeof(path), fp) != NULL) {
		if(print){
			printf("%s", path); 
		}
	}
	
	pclose(fp);
	
	if(strstr(path, syncAddr) != NULL)
	{
		if(print){
			printf("Time is synchronized to NTP server\n");
		}
		
		// following code gets the parameters
		char *substr;
		char space[2] = " ";
		char *token;
		int count = 1; 
			
		substr = strstr(path, syncAddr);
		token = strtok(substr, space);

		while( token!= NULL && count <= 9) {
			token = strtok(NULL, space);
			switch (count) {
				case 7:
					*delay = atof(token);
					break;
				case 8:
					*offset = atof(token);
					break;
				case 9:
					*jitter = atof(token);
					break;
				default:
					break;
			}
			count++;
		}
	
		if(print){
			printf("delay: %f\n", *delay);
			printf("offset: %f\n", *offset);
			printf("jitter: %f\n", *jitter);
		}
	
		return 1;

	} else {
		if(print){
			printf("Time is not synchronized to NTP server\n");
		}
		return 0;
	}
		
}

/*
void getNTPVals(double *delay, double *offset, double *jitter, int print) {
	FILE *fp;
	char path[1035];
	char *serverAddr = "192.168.42.1";
	char *substr;
	char space[2] = " ";
	char *token;
	int count = 1; 

	fp = popen("ntpq -pn", "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(1);
	}
	
	while (fgets(path, sizeof(path), fp) != NULL) {
		if(print){
			printf("%s", path);
		} 
	}
	
	pclose(fp);
		
	substr = strstr(path, serverAddr);
	token = strtok(substr, space);

	while( token!= NULL && count <= 9) {
		token = strtok(NULL, space);
		switch (count) {
			case 7:
				*delay = atof(token);
				break;
			case 8:
				*offset = atof(token);
				break;
			case 9:
				*jitter = atof(token);
				break;
			default:
				break;
		}
		count++;
	}
	
	if(print){
		printf("delay: %f\n", *delay);
		printf("offset: %f\n", *offset);
		printf("jitter: %f\n", *jitter);
	}
}
*/

#endif 



