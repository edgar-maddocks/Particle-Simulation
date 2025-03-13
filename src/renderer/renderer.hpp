#define GLM_ENABLE_EXPERIMENTAL
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../solver/solver.hpp"

class Renderer {
    public:
        Renderer(Solver& solver, int num_threads)
        : solver(solver)
        , num_threads(num_threads) 
        {}

        ~Renderer(){
            stop();
        }

        void start(){
            for (int i = 0; i < num_threads; ++i) {
                threads.emplace_back(&Renderer::renderLoop, this);
            }
        }

        void stop(){
            {
                std::unique_lock<std::mutex> lock(render_mutex);
                running = false;
                ready_to_render = true;
            }
            render_cv.notify_all();
            for (auto& thread : threads) {
                if (thread.joinable()){
                    thread.join();
                }
            }
        }

        void render(){
            if (solver.getBoundary()){
                solver.renderBoundary();
            }
            size_t num_objects = solver.getObjects().size();
            if (num_objects > 1000) {
                {
                    std::unique_lock<std::mutex> lock(render_mutex);
                    ready_to_render = true;
                    rendering_done = false;
                }
                render_cv.notify_one();
            
                {
                    std::unique_lock<std::mutex> lock(render_mutex);
                    render_cv.wait(lock, [this] { return rendering_done; });
                }
            } else {
                renderObjectsSubset(0, num_objects);
            }
        }

    private:
        Solver& solver;
        int num_threads;
        std::vector<std::thread> threads;
        std::mutex render_mutex;
        std::condition_variable render_cv;
        bool ready_to_render = false;
        bool rendering_done = false;
        bool running = true;

        void renderLoop(){
            while (running) {
                std::unique_lock<std::mutex> lock(render_mutex);
                render_cv.wait(lock, [this] { return ready_to_render || !running; });
        
                if (!running) break;
        
                size_t num_objects = solver.getObjects().size();
                size_t objects_per_thread = num_objects / num_threads;
                size_t start = 0;
                size_t end = 0;
        
                for (int i = 0; i < num_threads; ++i){
                    start = i * objects_per_thread;
                    end = (i == num_threads - 1) ? num_objects : start + objects_per_thread;
                    renderObjectsSubset(start, end);
                }
        
                if (solver.getBoundary()){
                    solver.renderBoundary();
                }
            }
        }

        void renderObjectsSubset(size_t start, size_t end) {
            for (size_t i = start; i < end; ++i) {
                solver.getObjects()[i].draw(50);
            }
        }
};

#endif