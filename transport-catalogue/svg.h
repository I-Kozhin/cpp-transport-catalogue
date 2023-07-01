#pragma once

/**
 * @file svg.h
 * @brief Contains the declaration of SVG-related classes and functions.
 */

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <variant>
#include <sstream>



namespace svg {

	/**
 	 * @struct Rgb
 	 * @brief Represents an RGB color.
 	 */
	class Rgb {
		public:
			Rgb() = default;
			Rgb(uint8_t red, uint8_t green, uint8_t blue);

			uint8_t red_ = 0;
			uint8_t green_ = 0;
			uint8_t blue_ = 0;
    };
    inline void print_color(std::ostream& out, Rgb& rgb);

	/**
 	 * @struct Rgba
 	 * @brief Represents an RGBA color.
 	 */
	class Rgba {
		public:
			Rgba() = default;
			Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity);

			uint8_t red_ = 0;
			uint8_t green_ = 0;
			uint8_t blue_ = 0;
			double opacity_ = 1.0;
    };


	using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;


	inline const Color NoneColor{ "none" };

	/**
	 * @struct Point
	 * @brief Represents a 2D point.
	 */
	struct Point {
		Point() = default;
		Point(double x, double y)
			: x(x)
			, y(y) {
		}
		double x = 0;
		double y = 0;
	};

	/**
	 * @struct RenderContext
	 * @brief Represents the rendering context.
	 */
	struct RenderContext {
		RenderContext(std::ostream& out)
			: out(out) {
		}

		RenderContext(std::ostream& out, int indent_step, int indent = 0)
			: out(out)
			, indent_step(indent_step)
			, indent(indent) {
		}

		/**
		 * @brief Creates a new RenderContext object with increased indentation.
		 * @return The new RenderContext object with increased indentation.
		 */
		RenderContext Indented() const {
			return { out, indent_step, indent + indent_step };
		}

		/**
		 * @brief Renders the current indentation level.
		 */
		void RenderIndent() const {
			for (int i = 0; i < indent; ++i) {
				out.put(' ');
			}
		}

		std::ostream& out;		///< The output stream used for rendering
		int indent_step = 0;
		int indent = 0;
	};

	/**
	 * @class Object
	 * @brief Represents an SVG object.
	 *
	 * The Object class is an abstract base class for all SVG objects.
	 * It provides a common interface for rendering the objects.
	 */
	class Object {
		public:
			/**
			 * @brief Renders the object using the provided rendering context.
			 * @param context The rendering context.
			 */
			void Render(const RenderContext& context) const;

			virtual ~Object() = default;

		private:

			/**
			 * @brief Renders the specific object using the provided rendering context.
			 * @param context The rendering context.
			 */
			virtual void RenderObject(const RenderContext& context) const = 0;
	};

	/**
	 * @enum StrokeLineCap
	 * @brief Represents the line cap styles for stroke rendering.
	 */
	enum class StrokeLineCap {
		BUTT,   ///< The line ends with no added shape.
		ROUND,  ///< The line ends with a round shape.
		SQUARE, ///< The line ends with a square shape.
	};

	/**
	 * @enum StrokeLineJoin
	 * @brief Represents the line join styles for stroke rendering.
	 */
	enum class StrokeLineJoin {
		ARCS,       ///< Used to create a sharp corner.
		BEVEL,      ///< Used to create a beveled corner.
		MITER,      ///< Used to create a pointed corner.
		MITER_CLIP, ///< Similar to miter, but with clipped line ends.
		ROUND,      ///< Used to create a rounded corner.
	};

	/**
	 * @brief Overload of the output stream operator for StrokeLineCap.
	 * @param os The output stream.
	 * @param cap The StrokeLineCap value.
	 * @return The output stream.
	 */
	std::ostream& operator<<(std::ostream& os, const StrokeLineCap& cap);

	/**
	 * @brief Overload of the output stream operator for StrokeLineJoin.
	 * @param os The output stream.
	 * @param join The StrokeLineJoin value.
	 * @return The output stream.
	 */
	std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& join);

	/**
 	 * @struct ColorPrinter
 	 * @brief Helper struct for printing different color types.
 	 */
	struct ColorPrinter {
		std::ostream& os;
		void operator()(std::monostate) const {
			os << std::string("none") /*<< std::endl*/;
		}
		void operator()(const std::string& s) const {
			os << s /*<< std::endl*/;
		}
		void operator()(const Rgb& r) const {
			os << "rgb(" << static_cast<unsigned int>(r.red_) << "," << static_cast<unsigned int>(r.green_) << "," << static_cast<unsigned int>(r.blue_) << ")" /*<< std::endl*/;
		}
		void operator()(const Rgba& r) const {
			os << "rgba(" << static_cast<unsigned int>(r.red_) << "," << static_cast<unsigned int>(r.green_) << "," << static_cast<unsigned int>(r.blue_) << "," << r.opacity_ << ")" /*<< std::endl*/;
		}
	};

	/**
	 * @brief Overload of the output stream operator for Color.
	 * @param os The output stream.
	 * @param color The Color value.
	 * @return The output stream.
	 */
	std::ostream& operator<<(std::ostream& os, const Color& color);

	/**
	 * @class PathProps
	 * @brief Provides common properties and methods for path-based objects.
	 * @tparam Owner The owner class that inherits from PathProps.
	 */
	template <typename Owner>
	class PathProps {
		public:
			Owner& SetFillColor(Color color) {
				fill_color_ = std::move(color);
				return AsOwner();
			}
			Owner& SetStrokeColor(Color color) {
				stroke_color_ = std::move(color);
				return AsOwner();
			}

			Owner& SetStrokeWidth(double width) {
				stroke_width_ = width;
				return AsOwner();
			}

			Owner& SetStrokeLineJoin(StrokeLineJoin Strok) {
				stroke_line_join_ = std::move(Strok);
				return AsOwner();
			}

			Owner& SetStrokeLineCap(StrokeLineCap Strok) {
				stroke_line_cap_ = std::move(Strok);
				return AsOwner();
			}


		protected:
			~PathProps() = default;

			void RenderAttrs(std::ostream& out) const {
				using namespace std::literals;



				if (fill_color_) {

					out << "fill=\""sv;

					std::visit(ColorPrinter{ out }, *fill_color_);
					out << "\""sv;
				}
				if (stroke_color_) {

					out << " stroke=\""sv;

					std::visit(ColorPrinter{ out }, *stroke_color_);
					out << "\""sv;
				}
				if (stroke_width_) {
					out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
				}
				if (stroke_line_cap_) {
					out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv;
				}
				if (stroke_line_join_) {
					out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
				}

			}

		private:
			Owner& AsOwner() {

				return static_cast<Owner&>(*this);
			}

			std::optional<Color> fill_color_;
			std::optional<Color> stroke_color_;
			std::optional<StrokeLineJoin> stroke_line_join_;
			std::optional<StrokeLineCap> stroke_line_cap_;
			std::optional<double> stroke_width_;
	};

	/**
	 * @class Circle
	 * @brief Represents an SVG circle object.
	 */
	class Circle final : public Object, public PathProps<Circle> {
		public:
			Circle& SetCenter(Point center);
			Circle& SetRadius(double radius);

		private:
			void RenderObject(const RenderContext& context) const override;

			Point center_;
			double radius_ = 1.0;
	};

	/**
	 * @class Polyline
	 * @brief Represents an SVG polyline object.
	 */
	class Polyline : public  Object, public PathProps<Polyline> {
		public:

			Polyline& AddPoint(Point point);

		private:
			std::vector<Point> points_;
			void RenderObject(const RenderContext& context) const override;
	};



	/**
	 * @class Text
	 * @brief Represents an SVG text object.
	 */
	class Text : public Object, public PathProps<Text> {
		public:

			Text& SetPosition(Point pos);

			Text& SetOffset(Point offset);

			Text& SetFontSize(uint32_t size);

			Text& SetFontFamily(std::string font_family);

			Text& SetFontWeight(std::string font_weight);

			Text& SetData(std::string data);

		private:
			double x, y;                   ///< The position coordinates of the text.
			double dx, dy;                 ///< The offset coordinates of the text.

			uint32_t font_size = 1; 
			std::string font_family_name; 
			std::string font_weight_type; 
			std::string text_data; 
			uint32_t width_;

			void RenderObject(const RenderContext& context) const override;

	};

	/**
	 * @class ObjectContainer
	 * @brief Represents a container for SVG objects.
	 */
	class ObjectContainer {
		public:
			template <typename Obj>
			void Add(Obj obj) {
				AddPtr(std::make_unique<Obj>(std::move(obj)));
			}

			virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

			virtual ~ObjectContainer() = default;
	};

	/**
	 * @class Drawable
	 * @brief Represents a drawable SVG object.
	 */
	class Drawable {
		public:

			/**
			 * @brief Draws the object into the specified object container.
			 * @param o The object container.
			 */
			virtual void Draw(ObjectContainer& o) const = 0;
			virtual ~Drawable() = default;
	};

	/**
	 * @class Document
	 * @brief Represents an SVG document.
	 */
	class Document : public ObjectContainer {
		public:

			void AddPtr(std::unique_ptr<Object>&& obj) override;

			/**
			 * @brief Renders the SVG document into the specified output stream.
			 * @param out The output stream.
			 */
			void Render(std::ostream& out) const;

			virtual ~Document() = default;

		private:
			std::vector<std::unique_ptr<Object>> objects_;
	};


}  // namespace svg
