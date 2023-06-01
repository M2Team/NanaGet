using Microsoft.VisualBasic;
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

            List<(string Release, string Preview)> ReplacementStringList =
                new List<(string Release, string Preview)> ();
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
                Content = Content.Replace(
                    ReplacementStringItem.Release,
                    ReplacementStringItem.Preview);
                //Content = Content.Replace(
                //    ReplacementStringItem.Preview,
                //    ReplacementStringItem.Release);
            }

            FileUtilities.SaveTextToFileAsUtf8Bom(FilePath, Content);

            Console.WriteLine("Hello, World!");
            Console.ReadKey();
        }
    }
}
