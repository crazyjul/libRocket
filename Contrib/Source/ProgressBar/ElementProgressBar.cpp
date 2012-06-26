#include <Rocket/ProgressBar/ElementProgressBar.h>
#include <Rocket/Core/GeometryUtilities.h>
#include <Rocket/Core/ElementDocument.h>

namespace Rocket {
namespace ProgressBar {

// Constructs a new ElementProgressBar. This should not be called directly; use the Factory instead.
ElementProgressBar::ElementProgressBar(const Rocket::Core::String& tag) : Core::Element(tag), start_geometry(this), center_geometry(this), end_geometry(this), geometry_dirty(true), value(0.0f), progressbar_orientation(ProgressBarOrientationLeft)
{
}

ElementProgressBar::~ElementProgressBar()
{
}

// Returns float value between 0 and 1 of the progress bar.
float ElementProgressBar::GetValue() const
{
	return GetAttribute< float >("value", 0.0f);
}

// Sets the current value of the progress bar.
void ElementProgressBar::SetValue(const float value)
{
	SetAttribute("value", value);
}

// Called during the update loop after children are rendered.
void ElementProgressBar::OnUpdate()
{
}

// Called during render after backgrounds, borders, decorators, but before children, are rendered.
void ElementProgressBar::OnRender()
{
	if(geometry_dirty)
		GenerateGeometry();

	start_geometry.Render(GetAbsoluteOffset(Rocket::Core::Box::CONTENT));
	center_geometry.Render(GetAbsoluteOffset(Rocket::Core::Box::CONTENT));
	end_geometry.Render(GetAbsoluteOffset(Rocket::Core::Box::CONTENT));
}

// Called when attributes on the element are changed.
void ElementProgressBar::OnAttributeChange(const Core::AttributeNameList& changed_attributes)
{
	Element::OnAttributeChange(changed_attributes);

	if (changed_attributes.find("value") != changed_attributes.end())
	{
		value = GetAttribute< float >("value", 0.0f);

		if (value < 0)
			value = 0.0f;
		else if (value > 1)
			value = 1.0f;

		geometry_dirty = true;
	}
}

// Called when properties on the element are changed.
void ElementProgressBar::OnPropertyChange(const Core::PropertyNameList& changed_properties)
{
	Element::OnPropertyChange(changed_properties);

	if (changed_properties.find("progress-start-src") != changed_properties.end())
	{
		LoadTexture(0);
	}
	
	if (changed_properties.find("progress-center-src") != changed_properties.end())
	{
		LoadTexture(1);
	}
	
	if (changed_properties.find("progress-end-src") != changed_properties.end())
	{
		LoadTexture(2);
	}

	if (changed_properties.find("orientation") != changed_properties.end())
	{
		Core::String orientation_string = GetProperty< Core::String >("orientation");

		if (orientation_string == "left")
		{
			progressbar_orientation = ProgressBarOrientationLeft;
		}
		else if (orientation_string =="right")
		{
			progressbar_orientation = ProgressBarOrientationRight;
		}
		else if (orientation_string == "top")
		{
			progressbar_orientation = ProgressBarOrientationTop;
		}
		else if (orientation_string == "bottom")
		{
			progressbar_orientation = ProgressBarOrientationBottom;
		}

		geometry_dirty = true;
	}
}

// Called when value has changed.
void ElementProgressBar::GenerateGeometry()
{
	const Core::Vector2f complete_extent = GetBox().GetSize(Core::Box::CONTENT);
	Core::Vector2f texcoords[4];
	Core::Vector2i initial_part_size[3];
	Core::Vector2f final_part_position[3];
	Core::Vector2f final_part_size[3];
	float progress_size;

	initial_part_size[0] = texture[0].GetDimensions(GetRenderInterface());
	initial_part_size[2] = texture[2].GetDimensions(GetRenderInterface());

	start_geometry.Release(true);
	center_geometry.Release(true);
	end_geometry.Release(true);

	texcoords[0] = Core::Vector2f(0, 0);
	texcoords[1] = Core::Vector2f(1, 0);
	texcoords[2] = Core::Vector2f(1, 1);
	texcoords[3] = Core::Vector2f(0, 1);

	switch(progressbar_orientation)
	{
		case ProgressBarOrientationLeft :
		{
			progress_size = value * (complete_extent.x - initial_part_size[0].x - initial_part_size[2].x);

			final_part_size[0].y = final_part_size[1].y = final_part_size[2].y = float(complete_extent.y);
			final_part_size[0].x = float(initial_part_size[0].x);
			final_part_size[2].x = float(initial_part_size[2].x);
			final_part_size[1].x = progress_size;

			final_part_position[0] = Core::Vector2f(0, 0);
			final_part_position[1] = Core::Vector2f(final_part_size[0].x, 0);
			final_part_position[2] = Core::Vector2f(final_part_size[0].x + final_part_size[1].x, 0);
		} break;

		case ProgressBarOrientationRight :
		{
			float
				offset;

			progress_size = value * (complete_extent.x - initial_part_size[0].x - initial_part_size[2].x);
			offset = (complete_extent.x - initial_part_size[0].x - initial_part_size[2].x) - progress_size;

			final_part_size[0].y = final_part_size[1].y = final_part_size[2].y = float(complete_extent.y);
			final_part_size[0].x = float(initial_part_size[0].x);
			final_part_size[2].x = float(initial_part_size[2].x);
			final_part_size[1].x = progress_size;

			final_part_position[0] = Core::Vector2f(offset, 0);
			final_part_position[1] = Core::Vector2f(final_part_size[0].x + offset, 0);
			final_part_position[2] = Core::Vector2f(final_part_size[0].x + final_part_size[1].x + offset, 0);
		} break;

		case ProgressBarOrientationTop :
		{
			progress_size = value * (complete_extent.y - initial_part_size[0].y - initial_part_size[2].y);

			final_part_size[0].x = final_part_size[1].x = final_part_size[2].x = float(complete_extent.x);
			final_part_size[0].y = float(initial_part_size[0].y);
			final_part_size[2].y = float(initial_part_size[2].y);
			final_part_size[1].y = progress_size;

			final_part_position[0] = Core::Vector2f(0, 0);
			final_part_position[1] = Core::Vector2f(0, final_part_size[0].y);
			final_part_position[2] = Core::Vector2f(0, final_part_size[0].y + final_part_size[1].y);
		} break;

		case ProgressBarOrientationBottom :
		{
			float
				offset;

			progress_size = value * (complete_extent.y - initial_part_size[0].y - initial_part_size[2].y);
			offset = (complete_extent.y - initial_part_size[0].y - initial_part_size[2].y) - progress_size;

			final_part_size[0].x = final_part_size[1].x = final_part_size[2].x = float(complete_extent.x);
			final_part_size[0].y = float(initial_part_size[0].y);
			final_part_size[2].y = float(initial_part_size[2].y);
			final_part_size[1].y = progress_size;

			final_part_position[2] = Core::Vector2f(0, offset);
			final_part_position[1] = Core::Vector2f(0, final_part_size[2].y + offset);
			final_part_position[0] = Core::Vector2f(0, final_part_size[2].y + final_part_size[1].y + offset);
		} break;
	}

	

	// Generate start part geometry.
	{
		std::vector< Rocket::Core::Vertex >& vertices = start_geometry.GetVertices();
		std::vector< int >& indices = start_geometry.GetIndices();

		vertices.resize(4);
		indices.resize(6);

		Core::GeometryUtilities::GenerateQuad(&vertices[0],
			&indices[0],
			final_part_position[0],
			final_part_size[0],
			Core::Colourb(255, 255, 255, 255),
			texcoords[0],
			texcoords[1]);
	}

	// Generate center part geometry.
	{
		std::vector< Rocket::Core::Vertex >& vertices = center_geometry.GetVertices();
		std::vector< int >& indices = center_geometry.GetIndices();

		vertices.resize(4);
		indices.resize(6);

		Core::GeometryUtilities::GenerateQuad(&vertices[0],
			&indices[0],
			final_part_position[1],
			final_part_size[1],
			Core::Colourb(255, 255, 255, 255),
			texcoords[0],
			texcoords[1]);
	}

	// Generate center part geometry.
	{
		std::vector< Rocket::Core::Vertex >& vertices = end_geometry.GetVertices();
		std::vector< int >& indices = end_geometry.GetIndices();

		vertices.resize(4);
		indices.resize(6);

		Core::GeometryUtilities::GenerateQuad(&vertices[0],
			&indices[0],
			final_part_position[2],
			final_part_size[2],
			Core::Colourb(255, 255, 255, 255),
			texcoords[0],
			texcoords[1]);
	}

	geometry_dirty = false;
}

/// Called when source texture has changed.
void ElementProgressBar::LoadTexture(int index)
{
	Core::ElementDocument* document = GetOwnerDocument();
	Core::URL source_url(document == NULL ? "" : document->GetSourceURL());

	switch(index)
	{
		case 0:
		{
			Core::String source = GetProperty< Core::String >("progress-start-src");

			if (!texture[0].Load(source, source_url.GetPath()))
			{
				start_geometry.SetTexture(NULL);
				return;
			}

			start_geometry.SetTexture(&texture[0]);
			break;
		}

		case 1:
		{
			Core::String source = GetProperty< Core::String >("progress-center-src");

			if (!texture[1].Load(source, source_url.GetPath()))
			{
				center_geometry.SetTexture(NULL);
				return;
			}

			center_geometry.SetTexture(&texture[1]);
			break;
		}

		case 2:
		{
			Core::String source = GetProperty< Core::String >("progress-end-src");

			if (!texture[2].Load(source, source_url.GetPath()))
			{
				end_geometry.SetTexture(NULL);
				return;
			}

			end_geometry.SetTexture(&texture[2]);
			break;
		}
	}
}

}
}
