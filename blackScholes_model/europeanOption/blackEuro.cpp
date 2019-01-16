/* 
======================================================
 Copyright 2016 Liang Ma

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
======================================================
*
* Author:   Liang Ma (liang-ma@polito.it)
*
* Top function is defined here with interface specified as axi4.
* It creates an object of blackScholes and launches the simulation.
*
*----------------------------------------------------------------------------
*/
#include "RNG.h"
#include "blackScholes.h"
#include "europeanOption.h"
#include "stockData.h"
#include "launchSim.h"

extern "C"
void blackEuro(float *pCall, float *pPut,   // call price and put price
		float timeT,				// time period of options
		float freeRate,			// interest rate of the riskless asset
		float volatility,			// volatility of the risky asset
		float initPrice,			// stock price at time 0
		float strikePrice,// strike price
		int steps,
		int sims,
		int g_id)			
{
#pragma HLS INTERFACE m_axi port=pCall bundle=gmem
#pragma HLS INTERFACE s_axilite port=pCall bundle=control
#pragma HLS INTERFACE m_axi port=pPut bundle=gmem
#pragma HLS INTERFACE s_axilite port=pPut bundle=control
#pragma HLS INTERFACE s_axilite port=timeT bundle=control
#pragma HLS INTERFACE s_axilite port=freeRate bundle=control
#pragma HLS INTERFACE s_axilite port=volatility bundle=control
#pragma HLS INTERFACE s_axilite port=initPrice bundle=control
#pragma HLS INTERFACE s_axilite port=strikePrice bundle=control
#pragma HLS INTERFACE s_axilite port=steps bundle=control
#pragma HLS INTERFACE s_axilite port=sims bundle=control
#pragma HLS INTERFACE s_axilite port=g_id bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS ALLOCATION instances=mul limit=1 operation
#pragma HLS ALLOCATION instances=fmul limit=1 operation
#pragma HLS ALLOCATION instances=fexp limit=1 operation
#pragma HLS ALLOCATION instances=fdiv limit=1 operation
	static const int SIMS_PER_GROUP =32;
	stockData<float> sd(timeT,freeRate,volatility,initPrice,strikePrice);
	
	float call0, put0;
	blackScholes<SIMS_PER_GROUP,EuropeanOptionStatus<float>, float> bs0(sd, steps);
	float call1, put1;
	blackScholes<SIMS_PER_GROUP,EuropeanOptionStatus<float>, float> bs1(sd, steps);
	{
#pragma HLS FUNCTION_EXTRACT
#pragma HLS DATAFLOW
		launchSimulation(call0, put0,  g_id, bs0, sims*steps>>1, sims>>1);
		launchSimulation(call1, put1,  g_id<<1, bs1, sims*steps>>1, sims>>1);
	}

	pCall[g_id] = (call0
			+call1
			)/sims;
	pPut[g_id] =(put0
			+put1
			)/sims;
}
