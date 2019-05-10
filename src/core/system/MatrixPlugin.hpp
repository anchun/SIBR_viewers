
// This file is used to extend Eigen's MatrixBase class using
// the following tricks:
// https://eigen.tuxfamily.org/dox-3.2/TopicCustomizingEigen.html

public:

typedef Scalar Type;
enum { NumComp = RowsAtCompileTime };

//Matrix( const Scalar* data ) { for(int i=0; i<NumComp; i++) this->operator [] (i) = data[i]; }

/**
Matrix( float x, float y=0.f, float z=0.f, float w=0.f ) {
	float data[] = {x, y, z, w};
	for(int i=0; i<NumComp; i++) this->operator [] (i) = data[i];
}
**/

