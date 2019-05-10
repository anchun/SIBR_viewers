
// This file is used to extend Eigen's MatrixBase class using
// the following tricks:
// https://eigen.tuxfamily.org/dox-3.2/TopicCustomizingEigen.html

public:

inline MatrixBase transposed( void ) { return this->transpose().eval(); }



Matrix<Scalar, 2, 1,Eigen::DontAlign>	xy( float fill=0.f ) const {
	return Matrix<Scalar, 2, 1,Eigen::DontAlign>( this->operator[](0), size()<2? fill:this->operator[](1));
}
Matrix<Scalar, 2, 1, Eigen::DontAlign>	yx(float fill = 0.f) const {
	return Matrix<Scalar, 2, 1, Eigen::DontAlign>(this->operator[](1), size()<2 ? fill : this->operator[](0));
}
Matrix<Scalar, 2, 1, Eigen::DontAlign>	wz(float fill = 0.f) const {
	return Matrix<Scalar, 2, 1, Eigen::DontAlign>(size()<4 ? fill : this->operator[](3), size()<3 ? fill : this->operator[](2));
}
Matrix<Scalar, 3, 1,Eigen::DontAlign>	xyz( float fill=0.f ) const {
	return Matrix<Scalar, 3, 1,Eigen::DontAlign>( this->operator[](0), size()<2? fill:this->operator[](1), size()<3? fill:this->operator[](2));
}
Matrix<Scalar, 4, 1,Eigen::DontAlign>	xyzw( float fill=0.f ) const {
	return Matrix<Scalar, 4, 1,Eigen::DontAlign>( this->operator[](0), size()<2? fill:this->operator[](1), size()<3? fill:this->operator[](2), size()<4? fill:this->operator[](3));
}
Matrix<Scalar, 3, 1, Eigen::DontAlign>	yxz(float fill = 0.f) const {
	return Matrix<Scalar, 3, 1, Eigen::DontAlign>(size()<2 ? fill : this->operator[](1), this->operator[](0), size()<3 ? fill : this->operator[](2));
}
Matrix<Scalar, 3, 1, Eigen::DontAlign>	yzx(float fill = 0.f) const {
	return Matrix<Scalar, 3, 1, Eigen::DontAlign>(size()<2 ? fill : this->operator[](1), size()<3 ? fill : this->operator[](2), this->operator[](0));
}

// // what is that non-intuitive things ? (for ordering in std::algos ?)
// bool operator() (const Vector<T, 2>& A, const Vector<T, 2>& B) const {
// 	if(A[0]<B[0]) { return true;}
// 	if(A[0]>B[0]) { return false;}
// 	return A[1]<B[1];
// }

bool	isNull( void ) const { 
	
	return (array() == 0).all();
	/*return (size()<1? true:this->operator[](0) ==Scalar(0))
		&& (size()<2? true:this->operator[](1) ==Scalar(0))
		&& (size()<3? true:this->operator[](2) ==Scalar(0))
		&& (size()<4? true:this->operator[](3) ==Scalar(0));*/

	//bool zeroDetected = false; 
	//for(int i=0; i<size(); ++i) 
	//	zeroDetected = zeroDetected | (this->operator [] (i) == 0);
	//return zeroDetected;
}

typedef Scalar Type;

//enum { NumComp = Derived::RowsAtCompileTime };

