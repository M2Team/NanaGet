using Mile.Project.Helpers;
using System.Text;

namespace DevelopmentChannelSwitcher
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string ProjectRootPath = GitRepository.GetRootPath();
            Console.WriteLine(ProjectRootPath);

            bool Preview = true;

            {
                List<(string Release, string Preview)> ReplacementStringList =
                new List<(string Release, string Preview)>();
                ReplacementStringList.Add((
                    "DisplayName=\"NanaGet\"",
                    "DisplayName=\"NanaGet Preview\""));
                ReplacementStringList.Add((
                    "Name=\"40174MouriNaruto.NanaGet\"",
                    "Name=\"40174MouriNaruto.NanaGetPreview\""));
                ReplacementStringList.Add((
                    "<DisplayName>NanaGet</DisplayName>",
                    "<DisplayName>NanaGet Preview</DisplayName>"));

                string FilePath = string.Format(
                    @"{0}\NanaGetPackage\Package.appxmanifest",
                    ProjectRootPath);

                string Content = File.ReadAllText(
                    FilePath,
                    Encoding.UTF8);
                foreach (var ReplacementStringItem in ReplacementStringList)
                {
                    if (Preview)
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Release,
                            ReplacementStringItem.Preview);
                    }
                    else
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Preview,
                            ReplacementStringItem.Release);
                    }
                }

                FileUtilities.SaveTextToFileAsUtf8Bom(FilePath, Content);
            }

            {
                List<(string Release, string Preview)> ReplacementStringList =
                new List<(string Release, string Preview)>();
                ReplacementStringList.Add((
                    "<Content Include=\"..\\Assets\\PackageAssets\\**\\*\">",
                    "<Content Include=\"..\\Assets\\PreviewPackageAssets\\**\\*\">"));

                string FilePath = string.Format(
                    @"{0}\NanaGetPackage\NanaGetPackage.wapproj",
                    ProjectRootPath);

                string Content = File.ReadAllText(
                    FilePath,
                    Encoding.UTF8);
                foreach (var ReplacementStringItem in ReplacementStringList)
                {
                    if (Preview)
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Release,
                            ReplacementStringItem.Preview);
                    }
                    else
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Preview,
                            ReplacementStringItem.Release);
                    }
                }

                FileUtilities.SaveTextToFileAsUtf8Bom(FilePath, Content);
            }

            {
                List<(string Release, string Preview)> ReplacementStringList =
                new List<(string Release, string Preview)>();
                ReplacementStringList.Add((
                    "NanaGet.ico",
                    "NanaGetPreview.ico"));

                string FilePath = string.Format(
                    @"{0}\NanaGet\NanaGetResources.rc",
                    ProjectRootPath);

                string Content = File.ReadAllText(
                    FilePath,
                    Encoding.Unicode);
                foreach (var ReplacementStringItem in ReplacementStringList)
                {
                    if (Preview)
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Release,
                            ReplacementStringItem.Preview);
                    }
                    else
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Preview,
                            ReplacementStringItem.Release);
                    }
                }

                File.WriteAllText(
                    FilePath,
                    Content,
                    Encoding.Unicode);
            }

            Console.WriteLine("Hello, World!");
            Console.ReadKey();
        }
    }
}
