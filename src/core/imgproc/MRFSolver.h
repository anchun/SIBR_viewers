#ifndef MRFSOLVER_H
#define MRFSOLVER_H

#include "Config.hpp"
#include <core/imgproc/mrf/graph.h>
#include <vector>
#include <iostream>
#include <functional>
#include <memory>

namespace sibr {

	
	/**
	\ingroup sibr_imgproc
	*/
	class SIBR_IMGPROC_EXPORT MRFSolver
	{

	private:
		typedef Graph<double, double, double> GraphType;

		std::vector<int> _labList;	//map the label_id to the actual labels

		std::vector<int> _labels;	//assign each node its current best label_id

		//for each variable, gives the list of its neighbor variables
		std::vector<std::vector<int> >* _neighborMap;

		//number of iterations in alpha expansion
		int _numIterations;

		//unaries only requiring label
		std::vector<double> _UnaryLabelOnly;

		//unaries requiring label and variable
		std::shared_ptr<std::function<double(int, int)> > _unaryFull;

		//pairwises only requiring labels
		std::vector<std::vector< double > >  _PairwiseLabelsOnly;

		//pairwises requiring labels and variables
		std::shared_ptr<std::function<double(int, int, int, int)> > _pairwiseFull;

		void buildGraphAlphaExp(int label_iteration_id);
		void buildGraphBinaryLabels(void);

		double _energy;

		GraphType* _graph;

		bool ignoreIsolatedNode;

		double unaryTotal(int p, int lp_id);
		double pairwiseTotal(int p, int q, int lp_id, int lq_id);

	public:
		MRFSolver(void);

		MRFSolver(std::vector<int> labels, std::vector<std::vector<int> >* neighborMap, int numIterations,
			std::shared_ptr<std::function<double(int)> > unaryLabelOnly,
			std::shared_ptr<std::function<double(int, int)> > unaryFull,
			std::shared_ptr<std::function<double(int, int)> > pairwiseLabelsOnly,
			std::shared_ptr<std::function<double(int, int, int, int)> > pairwiseFull
		);

		void alphaExpansion(void);
		void solveBinaryLabels(void);

		std::vector<int> getLabels(void);
		double getTotalEnergy(void);
		double computeEnergyU(void);
		double computeEnergyW(void);
		std::vector<double> getUnariesEnergies(void);
		std::vector<double> getEnergies(void);

		~MRFSolver(void);




	};

}

#endif // MRFSOLVER_H