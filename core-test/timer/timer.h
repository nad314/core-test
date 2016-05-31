namespace core {
	template <class _t> struct timer {
		std::chrono::high_resolution_clock::time_point t1, t2;
		std::chrono::duration<double> elapsed;
		
		inline void start() {
			t1 = std::chrono::high_resolution_clock::now();
		}
		
		inline void stop() {
			t2 = std::chrono::high_resolution_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		}

		inline timer& update() { stop(); return *this; }
		
		inline _t ms(){return static_cast<_t>(elapsed.count()*1000);}
		inline _t s(){return  static_cast<_t>(elapsed.count());}
		inline _t m(){return  static_cast<_t>(elapsed.count()/60);}
		inline _t h(){return  static_cast<_t>(elapsed.count()/3600);}
		
		inline operator _t(){return ms();}
		
	};
};
