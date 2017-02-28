/*
 * ACMachine.cpp
 *
 *  Created on: 2017/02/27
 *      Author: sin
 */

#include "ACMachine.h"

/*
	std::set<const std::string> patterns;
	std::vector<std::map<alphabet,state>> transitions;
	std::vector<state> failures;
	std::vector<std::set<const std::string *> > output;

	state current;
 *
 */

ACMachine::ACMachine(void) {
	initialize();
	pattern.clear();
}

void ACMachine::initialize(void) {
	transition.clear();
	transition.push_back(std::map<alphabet,state>());
	resetState();
	failure.clear();
	failure.push_back(current);
	// failure to initial state from initial state eats up one symbol at transition.
	output.clear();
	output.push_back(std::set<const std::string *>());
	output[current].clear();
}

bool ACMachine::transfer(const alphabet & c) {
	std::map<alphabet,state>::iterator itp;
	itp = transition[current].find(c);
	if ( itp == transition[current].end() )
		return false;
	current = itp->second;
	return true;
}

ACMachine::state ACMachine::addPath(const std::string & patt) {
	uint32 pos;
	state nwstate;

	for(pos = 0; pos < patt.length(); ++pos) {
		if ( !transfer(patt[pos]) ) {
			nwstate = transition.size();
			transition.push_back(std::map<alphabet,state>());
			(transition[current])[patt[pos]] = nwstate;
			failure.push_back(0);
			output.push_back(std::set<const std::string *>());
			current = nwstate;
		}
	}
	return current;
}

bool ACMachine::addOutput(const std::string & patt) {
	std::pair<std::set<const std::string>::iterator, bool> result;
	result = pattern.insert(patt);
	const std::string & orgstr = *result.first;
	if ( result.second ) {
		output[current].insert( &orgstr );
		return true;
	}
	return false;
}

void ACMachine::addFailures() {

}

std::vector<std::pair<uint32, const std::string &> >
	ACMachine::search(const std::string & pattern) {
	std::vector<std::pair<uint32, const std::string &> > occurrs;

	uint32 pos = 0;
	resetState();
	while ( pos < pattern.size() ) {
		if ( transfer(pattern[pos]) ) {
			if ( output[current].size() > 0 ) {
				for(std::set<const std::string *>::iterator it = output[current].begin();
						it != output[current].end(); it++) {
					occurrs.push_back(std::pair<uint32,const std::string&>(pos,**it));
				}
			}
			pos++;
			continue;
		}
		// failure loop
		current = failure[current];
		if ( current == initialState ) {
			pos++;
		}
	}
	return occurrs;
}

std::ostream & ACMachine::printOn(std::ostream & out) const {
	out << "ACMachine(";
	for(state i = 0; i < size(); ++i ) {
		out << "<" << i << ">";
		if ( output[i].size() > 0 ) {
			out << "{";
			for(std::set<const std::string *>::iterator it = output[i].begin();
					it != output[i].end(); ) {
				out << "'" << **it << "'";
				if ( ++it != output[i].end() )
					out << ", ";
			}
			out << "} ";
		}
		out << "[";
		for(std::map<alphabet,state>::const_iterator it = transition[i].begin();
				it != transition[i].end(); it++) {
			out << "'" << (char)it->first << "'-> " << it->second << ", ";
		}
		out << "~> " << failure[i];
		out << "], ";
	}
	out << ") ";
	return out;
}
