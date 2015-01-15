// Parker Stebbins/1.13.2015

#pragma once

#include <cmath>

namespace dreadbot {
	template <typename T>
	class Vector2 {
	public:
		T x, y;

		// Default constructor
		Vector2(T _x = 0, T _y = 0) {
			x = _x;
			y = _y;
		}

		// Addition
		template <typename T0, typename T1> // Vector<A> + Number<B> -> Vector'<A>
		friend Vector2 <T1> operator + (Vector2 <T1> v, T0 n) {
			return Vector2<T1>(v.x + n, v.y + n);
		}
		template <typename T0, typename T1> // Number<A> + Vector<B> -> Vector'<B>
		friend Vector2 <T1> operator + (T0 n, Vector2 <T1> v) {
			return Vector2<T1>(n + v.x, n + v.y);
		}
		template <typename T0, typename T1> // Vector<A> + Vector<B> -> Vector'<A>
		friend Vector2 <T1> operator + (Vector2 <T0> v0, Vector2 <T1> v1) {
			return Vector2<T1>(v0.x + v1.x, v0.y + v1.y);
		}

		// Subtraction
		template <typename T0, typename T1> // Vector<A> - Number<B> -> Vector'<A>
		friend Vector2 <T1> operator - (Vector2 <T1> v, T0 n) {
			return Vector2<T1>(v.x - n, v.y - n);
		}
		template <typename T0, typename T1> // Number<A> - Vector<B> -> Vector'<B>
		friend Vector2 <T1> operator - (T0 n, Vector2 <T1> v) {
			return Vector2<T1>(n - v.x, n - v.y);
		}
		template <typename T0, typename T1> // Vector<A> - Vector<B> -> Vector'<A>
		friend Vector2 <T1> operator - (Vector2 <T0> v0, Vector2 <T1> v1) {
			return Vector2<T1>(v0.x - v1.x, v0.y - v1.y);
		}

		// Multiplication
		template <typename T0, typename T1> // Vector<A> * Number<B> -> Vector'<A>
		friend Vector2 <T1> operator * (Vector2 <T1> v, T0 n) {
			return Vector2<T1>(v.x * n, v.y * n);
		}
		template <typename T0, typename T1> // Number<A> * Vector<B> -> Vector'<B>
		friend Vector2 <T1> operator * (T0 n, Vector2 <T1> v) {
			return Vector2<T1>(n * v.x, n * v.y);
		}
		template <typename T0, typename T1> // Vector<A> * Vector<B> -> Vector'<A>
		friend Vector2 <T1> operator * (Vector2 <T0> v0, Vector2 <T1> v1) {
			return Vector2<T1>(v0.x * v1.x, v0.y * v1.y);
		}

		// Division
		template <typename T0, typename T1> // Vector<A> / Number<B> -> Vector'<A>
		friend Vector2 <T1> operator / (Vector2 <T1> v, T0 n) {
			return Vector2<T1>(v.x / n, v.y / n);
		}
		template <typename T0, typename T1> // Number<A> / Vector<B> -> Vector'<B>
		friend Vector2 <T1> operator / (T0 n, Vector2 <T1> v) {
			return Vector2<T1>(n / v.x, n / v.y);
		}
		template <typename T0, typename T1> // Vector<A> / Vector<B> -> Vector'<A>
		friend Vector2 <T1> operator / (Vector2 <T0> v0, Vector2 <T1> v1) {
			return Vector2<T1>(v0.x / v1.x, v0.y / v1.y);
		}

		/**
		 * @description Calculate the length of the vector
		*/
		double getMagnitude() {
			return std::sqrt(x*x + y*y);
		}

		/**
		 * @description Set the length of the vector
		*/
		void setMagnitude(double mag) {
			copy((*this)*(mag/getMagnitude()));
		}

		/**
		 * @description Convert to a unit vector
		*/
		void normalize() {
			double m = getMagnitude();
			x /= m;
			y /= m;
		}

		/**
		 * @description Rotate the vector around (0, 0).
		 * @param theta Rotation angle
		*/
		void rotate(double theta) {
			theta *= 0.01745329251; // Convert to radians
			double cosA = std::cos(theta);
			double sinA = std::sin(theta);
			double xOut = x*cosA - y*sinA;
			double yOut = x*sinA + y*cosA;
			x = xOut;
			y = yOut;
		}
		/**
		 * @description Set each field in one statement.
		 * @param x The x-value
		 * @param y The y-value
		*/
		void set(T _x, T _y) {
			x = _x;
			y = _y;
		}
		/**
		 * @description Copies the elements of C into the vector
		 * @param c The vector to copy
		*/
		void set(Vector2<T> c) {
			set(c.x, c.y);
		}
	}; // end vector2
} // end dreadbot namespace
