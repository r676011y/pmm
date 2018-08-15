/*
 * ACMachine.h
 *
 *  Created on: 2017/02/27
 *      Author: sin
 */

#ifndef ACMACHINE_ARRAY_H_
#define ACMACHINE_ARRAY_H_

#include <iostream>
#include <string>
#include <vector>
//#include <array>
#include <algorithm>
//#include <set>
//#include <map>

#include <cinttypes>
//#include <cstring>

typedef int32_t int32;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef size_t position;

class ACMachine {
public:
	typedef int32 state_index;
	typedef uint8 alphabet;
//	typedef const std::vector<alphabet> ustring;

	static const uint32 alphabet_size = 1<<(sizeof(alphabet)*8);

private:
	struct aspair {
		alphabet label;
		state_index state;

		aspair(const alphabet c) : label(c), state(State::undefined) {}
		aspair(const alphabet c, const state_index nstate) : label(c), state(nstate) {}
	};
	struct ascompare {
		bool operator() (const aspair & left, const aspair & right) {
			return left.label < right.label;
		}
	};
	struct State {
		std::vector<aspair> trans;
		state_index failure;
		std::vector< position > output;

		enum {
			initial = 0,
			undefined = (state_index) -1,
		};


		State() {
			trans.clear();
			failure = initial;
			output.clear();
		}

		state_index transition(const alphabet c) const {
			ascompare comp;
			aspair dummypair(c);
			std::vector<aspair>::const_iterator itr = std::lower_bound(trans.begin(), trans.end(), dummypair, comp);
			if ( itr == trans.end() )
				return State::undefined;
			else if ( itr->label != c )
				return State::undefined;
			return itr->state;
		}

		state_index transition(const alphabet c, state_index state) {
			ascompare comp;
			aspair dummypair(c);
			std::vector<aspair>::iterator itr = std::lower_bound(trans.begin(), trans.end(), dummypair, comp);
			if ( itr == trans.end()) {
				trans.insert(itr, aspair(c,state));//trans.push_back(aspair(c,state));
			} else if ( itr->label != c ) {
				trans.insert(itr, aspair(c,state));
			} else {
				return itr->state = state;
			}
			return state;
		}

		std::vector<aspair>::const_iterator nextTransition(const alphabet c) const {
			ascompare comp;
			aspair dummypair(c);
			std::vector<aspair>::const_iterator itr = std::lower_bound(trans.begin(), trans.end(), dummypair, comp);
			return itr;
		}
		friend std::ostream & operator<<(std::ostream & out, const State & sta) {
			return out;
		}

	};
	std::vector<State> states;
	state_index current;

private:
	std::ostream & printStateOn(std::ostream & out, state_index i, const std::string & pathstr) const;

	void setupInitialState(void);
	state_index initialState() const { return State::initial; }
	bool transfer(const alphabet & c, const bool ignore_case = false);

	state_index transition(const state_index s, const alphabet c) const;

public:
	ACMachine(void);

	uint32 stateCount() const { return states.size(); }

	state_index resetState() { return current = State::initial; }
	state_index currentState() const { return current; }

	bool atInitialState() const { return current == State::initial; }

	const std::vector<position> & currentOutput() const { return states[current].output; }

	// add patt to the trie and output of the destination state.
	template <typename T>
		state_index addPath(const T & patt, const uint32 & length);
	template <typename T>
		state_index addPath(const T patt[]);
	template <typename T>
		state_index addPath(const T & patt);

	template <typename T>
		bool addOutput(const T & patt);
	template <typename T>
		bool addOutput(const T patt[]);

	void addFailures();

	template <typename C>
	void addPatterns(const C & strset) {
		for(auto str : strset ) {
			resetState();
			addPath(str);
			addOutput(str);
		}
		addFailures();
	}

	std::vector<std::pair<position,const std::string>>
	search(const std::string & text);
	bool read(const alphabet & c, const bool ignore_case = false);

	std::ostream & printOn(std::ostream & out) const;

	friend std::ostream & operator<<(std::ostream & out, const ACMachine & acm) {
		return acm.printOn(out);
	}

};

#endif /* ACMACHINE_ARRAY_H_ */
